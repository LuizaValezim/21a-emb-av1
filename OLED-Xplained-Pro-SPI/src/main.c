#include <asf.h>

#include "oled/gfx_mono_ug_2832hsweg04.h"
#include "oled/gfx_mono_text.h"
#include "oled/sysfont.h"

#define LED_1_PIO PIOA
#define LED_1_PIO_ID ID_PIOA
#define LED_1_IDX 0
#define LED_1_IDX_MASK (1 << LED_1_IDX)

#define LED_2_PIO PIOC
#define LED_2_PIO_ID ID_PIOC
#define LED_2_IDX 30
#define LED_2_IDX_MASK (1 << LED_2_IDX)

#define LED_3_PIO PIOB
#define LED_3_PIO_ID ID_PIOB
#define LED_3_IDX 2
#define LED_3_IDX_MASK (1 << LED_3_IDX)

#define BUT_1_PIO PIOD
#define BUT_1_PIO_ID ID_PIOD
#define BUT_1_IDX 28
#define BUT_1_IDX_MASK (1u << BUT_1_IDX)

#define BUT_2_PIO PIOC
#define BUT_2_PIO_ID ID_PIOC
#define BUT_2_IDX 31
#define BUT_2_IDX_MASK (1u << BUT_2_IDX)

#define BUT_3_PIO PIOA
#define BUT_3_PIO_ID ID_PIOA
#define BUT_3_IDX 19
#define BUT_3_IDX_MASK (1u << BUT_3_IDX)

volatile char BUT1_flag = 0;
volatile char BUT2_flag = 0;
volatile char BUT3_flag = 0;
volatile char RTT_flag = 0;

void but1_callback(void)
{
	if (!pio_get(BUT_1_PIO, PIO_INPUT, BUT_1_IDX_MASK)) {
		// PINO == 1 --> Borda de subida
		BUT1_flag = 1;
	}
	else {
		BUT1_flag = 0;
		// PINO == 0 --> Borda de descida
	}
}


void but2_callback(void)
{
	if (!pio_get(BUT_2_PIO, PIO_INPUT, BUT_2_IDX_MASK)) {
		// PINO == 1 --> Borda de subida
		BUT2_flag = 1;
	}
	else {
		BUT2_flag = 0;
		// PINO == 0 --> Borda de descida
	}
}

void but3_callback(void)
{
	if (!pio_get(BUT_3_PIO, PIO_INPUT, BUT_3_IDX_MASK)) {
		// PINO == 1 --> Borda de subida
		BUT3_flag = 1;
	}
	else {
		BUT3_flag = 0;
		// PINO == 0 --> Borda de descida
	}
}

void io_init(void) {
  pmc_enable_periph_clk(LED_1_PIO_ID);
  pmc_enable_periph_clk(LED_2_PIO_ID);
  pmc_enable_periph_clk(LED_3_PIO_ID);
  pmc_enable_periph_clk(BUT_1_PIO_ID);
  pmc_enable_periph_clk(BUT_2_PIO_ID);
  pmc_enable_periph_clk(BUT_3_PIO_ID);

  pio_configure(LED_1_PIO, PIO_OUTPUT_0, LED_1_IDX_MASK, PIO_DEFAULT);
  pio_configure(LED_2_PIO, PIO_OUTPUT_0, LED_2_IDX_MASK, PIO_DEFAULT);
  pio_configure(LED_3_PIO, PIO_OUTPUT_0, LED_3_IDX_MASK, PIO_DEFAULT);

  pio_configure(BUT_1_PIO, PIO_INPUT, BUT_1_IDX_MASK, PIO_PULLUP| PIO_DEBOUNCE);
  pio_configure(BUT_2_PIO, PIO_INPUT, BUT_2_IDX_MASK, PIO_PULLUP| PIO_DEBOUNCE);
  pio_configure(BUT_3_PIO, PIO_INPUT, BUT_3_IDX_MASK, PIO_PULLUP| PIO_DEBOUNCE);

  pio_handler_set(BUT_1_PIO, BUT_1_PIO_ID, BUT_1_IDX_MASK, PIO_IT_FALL_EDGE,
  but1_callback);
  pio_handler_set(BUT_2_PIO, BUT_2_PIO_ID, BUT_2_IDX_MASK, PIO_IT_FALL_EDGE,
  but2_callback);
  pio_handler_set(BUT_3_PIO, BUT_3_PIO_ID, BUT_3_IDX_MASK, PIO_IT_FALL_EDGE,
  but3_callback);

  pio_enable_interrupt(BUT_1_PIO, BUT_1_IDX_MASK);
  pio_enable_interrupt(BUT_2_PIO, BUT_2_IDX_MASK);
  pio_enable_interrupt(BUT_3_PIO, BUT_3_IDX_MASK);

  pio_get_interrupt_status(BUT_1_PIO);
  pio_get_interrupt_status(BUT_2_PIO);
  pio_get_interrupt_status(BUT_3_PIO);

  NVIC_EnableIRQ(BUT_1_PIO_ID);
  NVIC_SetPriority(BUT_1_PIO_ID, 4);

  NVIC_EnableIRQ(BUT_2_PIO_ID);
  NVIC_SetPriority(BUT_2_PIO_ID, 4);

  NVIC_EnableIRQ(BUT_3_PIO_ID);
  NVIC_SetPriority(BUT_3_PIO_ID, 4);
}

volatile double tempo_rtt = 0;

static void RTT_init(float freqPrescale, uint32_t IrqNPulses, uint32_t rttIRQSource) {

	uint16_t pllPreScale = (int) (((float) 32768) / freqPrescale);
	
	rtt_sel_source(RTT, false);
	rtt_init(RTT, pllPreScale);
	
	if (rttIRQSource & RTT_MR_ALMIEN) {
		uint32_t ul_previous_time;
		ul_previous_time = rtt_read_timer_value(RTT);
		while (ul_previous_time == rtt_read_timer_value(RTT));
		rtt_write_alarm_time(RTT, IrqNPulses+ul_previous_time);
	}

	/* config NVIC */
	NVIC_DisableIRQ(RTT_IRQn);
	NVIC_ClearPendingIRQ(RTT_IRQn);
	NVIC_SetPriority(RTT_IRQn, 4);
	NVIC_EnableIRQ(RTT_IRQn);

	/* Enable RTT interrupt */
	if (rttIRQSource & (RTT_MR_RTTINCIEN | RTT_MR_ALMIEN))
	rtt_enable_interrupt(RTT, rttIRQSource);
	else
	rtt_disable_interrupt(RTT, RTT_MR_RTTINCIEN | RTT_MR_ALMIEN);
}

void RTT_Handler(void) {
	uint32_t ul_status;

	/* Get RTT status - ACK */
	ul_status = rtt_get_status(RTT);

	/* IRQ due to Time has changed */
	if ((ul_status & RTT_SR_ALMS) == RTT_SR_ALMS) {
		//pin_toggle(LED_PI2, LED_PI2_IDX_MASK);    // BLINK Led
		RTT_flag = 0;
		gfx_mono_draw_string("             ", 0, 0, &sysfont);
		gfx_mono_draw_string("             ", 0, 15, &sysfont);
	}
}

int main(void) {
  board_init();
  sysclk_init();
  delay_init();
  io_init();
  gfx_mono_ssd1306_init();
		
  int gabarito[] = {3, 3, 3, 3};
  int tentativa[] = {0, 0, 0, 0};
  int nTentativas = 0;
  int lenght = 0;
  int contador = 0;
  
  while (1) {
    if (!RTT_flag) {
		if (nTentativas > 0) {
			gfx_mono_draw_string("Cofre Fechado", 0, 0, &sysfont);
  
			if (BUT1_flag) {
				tentativa[contador] = 1;
				gfx_mono_draw_string("*", lenght, 15, &sysfont);
				BUT1_flag = 0;
		
				lenght += 10;
				contador += 1;
			}
	
			if (BUT2_flag) {
				tentativa[contador] = 2;
				gfx_mono_draw_string("*", lenght, 15, &sysfont);
				BUT2_flag = 0;
		
				lenght += 10;
				contador += 1;
			}
	
			if (BUT3_flag) {
				tentativa[contador] = 3;
				gfx_mono_draw_string("*", lenght, 15, &sysfont);
				BUT3_flag = 0;
		
				lenght += 10;
				contador += 1;
			}
	
			if (contador == 4) {
				gfx_mono_draw_string("             ", 0, 0, &sysfont);
				gfx_mono_draw_string("             ", 0, 15, &sysfont);
			
				if (tentativa[0] == gabarito[0] && tentativa[1] == gabarito[1] && tentativa[2] == gabarito[2] && tentativa[3] == gabarito[3]) {
					gfx_mono_draw_string("Cofre Aberto!", 0, 0, &sysfont);
					pio_set(LED_1_PIO, LED_1_IDX_MASK);
					pio_set(LED_2_PIO, LED_2_IDX_MASK);
					pio_set(LED_3_PIO, LED_3_IDX_MASK);
				}
				else {
					gfx_mono_draw_string("Bloqueado", 0, 0, &sysfont);
					gfx_mono_draw_string("Aguarde...", 0, 15, &sysfont);
					   
					RTT_init(4, 16, RTT_MR_ALMIEN);
					RTT_flag = 1;
				}
		
				nTentativas += 1;
				contador = 0;
				lenght = 0;
			}
		}
	
		else {
			gfx_mono_draw_string("Nova senha:", 0, 0, &sysfont);
		
			if (BUT1_flag) {
				gabarito[contador] = 1;
				gfx_mono_draw_string("1", lenght, 15, &sysfont);
				BUT1_flag = 0;
			
				lenght += 10;
				contador += 1;
			}
		
			if (BUT2_flag) {
				tentativa[contador] = 2;
				gfx_mono_draw_string("2", lenght, 15, &sysfont);
				BUT2_flag = 0;
			
				lenght += 10;
				contador += 1;
			}
		
			if (BUT3_flag) {
				tentativa[contador] = 3;
				gfx_mono_draw_string("3", lenght, 15, &sysfont);
				BUT3_flag = 0;
			
				lenght += 10;
				contador += 1;
			}
		
			if (contador == 4) {
				gfx_mono_draw_string("             ", 0, 0, &sysfont);
				gfx_mono_draw_string("             ", 0, 15, &sysfont);
			
				gfx_mono_draw_string("Senha Definida", 0, 0, &sysfont);
				pio_clear(LED_1_PIO, LED_1_IDX_MASK);
				pio_clear(LED_2_PIO, LED_2_IDX_MASK);
				pio_clear(LED_3_PIO, LED_3_IDX_MASK);
				
				gfx_mono_draw_string("             ", 0, 0, &sysfont);
				gfx_mono_draw_string("             ", 0, 15, &sysfont);
			
				contador = 0;
				lenght = 0;
				nTentativas = 1;
			}
		}
	  }
  }
}

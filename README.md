# 21a - Avaliação Prática 1

Leia atentamente as informações aseguir:

1. Atualize o arquivo `ALUNO.json` com seu nome e e-mail e faça um commit imediatamente.
1. **Faça o seu trabalho de maneira ética!**
1. **A cada 30 minutos você deverá fazer um commit no seu código!**
    - Códigos que não tiverem commit a cada 30min ou que mudarem drasticamente entre os commits serão descartados (conceito I) !!
    - Você deve inserir mensagens condizentes nos commits!
1. Duração: 3h
1. Usar o código exemplo deste repositório, ele já possui:
    - botões com interrupção
    - leds 
    - diplay do módulo OLED1
    - TC/ RTT e RTC adicionados no wizard

:triangular_flag_on_post: **Ao finalizar a entrega preencher o formulário (uma única vez!): https://docs.google.com/forms/d/e/1FAIpQLSf5zNFzO8V2a8Db53aqNy4QmJU2UQbc5kV0Cdrh7E73m9SD9A/viewform?usp=sf_link**

Utilize o projeto exemplo que se encontra neste repositório, nele os 

## Descrição

Nessa avaliação iremos trabalhar com o OLED1 e iremos desenvolver um protótipo de um  **cofre digital**. 
![](figs/cofre.png)

> Figura meramente ilustrativa =) [src](https://www.celeti.com.br/cofre-unee-classic-keypad-ucdck)

## C

O protótipo deve possuir o seguinte comportamento:

- A senha é definida por uma sequência dos botões 1, 2 e 3 da placa OLED (B1, B2 e B3).
- O cofre deve possuir uma senha fixa de tamanho 4 definido previamente por: `[B1 B1 B2 B3]`.
- Para cada vez que um botão for apertado deve exibir um `*` no display.
- Se a senha estiver correta, abre o cofre.
- Senha errada:
    - Bloqueia os botões por 4 segundos (deve-se usar **RTT**).
    - Exibe no LCD: `Senha errada` / `Bloqueado`.
- Cofre fechado:
    - Exibe no LCD: `Cofre Trancado`.
    - Todos os LEDs acesos.
- Cofre aberto:
    - Exibe no LCD: `Cofre Aberto`.
    - Todos os LEDs apagados.
- Para trancar o cofre é necessário apertar o botão 1.

Requisitos de firmware:

- Botões funcionando com interrupção.
- Usa RTT para timeout de 4s.

Assista ao vídeo no youtube com a implementação anterior:

[![Youtube](https://img.youtube.com/vi/HHSjHqWFiXU/0.jpg)](https://youtu.be/HHSjHqWFiXU)


### Itens extras 

Os itens a seguir fornecem meio ou um conceito a mais, você pode escolher qual quer fazer.

#### (+0.5 conceito)

Cada item a seguir fornece meio conceito a mais na nota.

- Possibilita usuário definir a senha (quando ligar a placa)
- Se errar a senha pela segunda vez seguida bloqueia por 10 segundos
- Uma vez o cofre aberto, para fechar é necessário apertar (e segurar) o botão 1 por alguns segundos
- Senha padrão com 6 digitos (`[B1 B1 B2 B2 B3 B1]`).
- Pisca os Leds enquanto a senha estiver bloqueada (**Usar TC**).

#### (+1 conceito)

Cada item a seguir fornece um conceito a mais na nota.

- A senha possui um timeout de 4 segundos, intervalo máximo entre o usuário apertar o próximo botão, estourado o tempo a senha é zerada e começa tudo novamente.
- Adiciona uma mensagem indicando quando que o cofre foi aberto: `dd:mm:aa / hh:mm` (**Usar RTC**)

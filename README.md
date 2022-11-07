# Projeto Embarcados

Desenvolvendo um controle remoto.

## Entrega 1

### Integrantes

- Diogo Duarte
- Jorás Oliveira

### Ideia

Controle para jogos de ritmo, ala DDR. Como por exemplo jogos como Guitar Hero, StepMania, Friday Night Funkin'.

### Nome

EmbarcaFunky

### Usuários 

Fãs de jogos de ritmo, que estão tendo dificuldade de achar um controle específico para jogar estes jogos de ritmo. Eles também possuem alguma noção de produzir um controle, caso queiram utilizar o nosso como base, ou apenas copiar o projeto.

### Software/Jogo 

Para o projeto, criaremos um controle para jogar o jogo StepMania. Stepmania é um jogo de ritmo, em que o jogador terá que acertar as setas no momento certo para obter a pontuação dela. Como é um jogo disponível pela internet, possuem diversos mods, em que você consegue jogar com a música que queira. Então olhando por esse ponto de vista, o acervo de músicas para serem jogadas é infinita.

### Jornada do usuários (3 pts)

<!-- Descreva ao menos duas jornadas de usuários distintos, é para caprichar! -->
O usuário quer ter uma nova experiência com um jogo familiar e criar conteúdo que pode ser interessante para outras pessoas.

O usuário quer jogar um jogo de ritmo que tem 4 botões como input, com um controle feito para isto.

João, um aluno de faculdade de música, sempre jogou jogos de ritmo, ele adora o estilo de DDR, mais especificamente Clone Hero, e recentemente descobriu StepMania. Como sempre jogou este tipo de jogos com o controle de guitarra que ganhou de seus pais, ao experimentar StepMania percebeu que sua antiga guitarra não seria útil e sentiu que não tinha um controle ideal para jogar. Com isso, João procurou até achar o controle que faremos. Os fatos que chamaram a sua atenção no controle foram que os 4 botões que se assemelhavam a sua antiga guitarra, leds que acendiam ao apertar os botões, e um joystick para conseguir navegar no menu do jogo, algo que é que não tinha em sua antiga guitarra.

Maria jogava Guitar Hero no Wii quando era menor. Sempre jogou jogos que tinham ritmos e gostava de se sacudir jogando eles. Recentemente foi introduzida a StepMania por sua namorada, e adorou, porém não se adaptou a jogar no teclado do computador de sua namorada. Pensando no seu tempo de Wii, começou a querer um controle próprio que funcionasse bem para o jogo, para que tenha fácil acesso aos botões para serem apertados e consiga se mexer mais ao jogar o jogo de música. Com isso, replicou nosso projeto, Maria gostou muito das cores dos leds piscando junto com os botões sendo apertados, e gostou da disposição dos botões, em que conseguia jogar e dançar ao mesmo tempo.

### Comandos/ Feedbacks (2 pts)

<!-- 
Quais são os comandos/ operacões possíveis do seu controle?

Quais os feedbacks que seu controle vai fornecer ao usuário?
-->

O controle terá 4 botões para jogar, em que cada está referente a seta para cima, esquerda, direite e baixo. A cada pressionada destes 4 botões, haverá um Rumble que causará tremor no controle, aumentando a imersão do jogo.
Terá 1 botão e analógico para navegação dentro do menu do jogo. 

Comandos:
 - 4 botões para jogar, cada um referente a seta do jogo
 - 1 botão de navegação
 - 1 joystick para movimentação no menu
 
 Feedback:
 - Rumble que irá tremer o controle a cada vez que um botão ser apertado, aumentando a imersão do jogo.

## In/OUT (3 pts)

<!--
Para cada Comando/ Feedback do seu controle, associe qual sensores/ atuadores pretende utilizar? Faca em formato de lista, exemplo:

- Avanca música: Push button amarelo
- Volume da música: Fita de LED indicando potência do som
-->

- Seta para cima: Push botão verde
- Rumble: Treme o controle

- Seta para baixo: Push botão vermelho
- Rumble: Treme o controle

- Seta para esquerda: Push botão amarelo
- Rumble: Treme o controle

- Seta para direita: Push botão Azul
- Rumble: Treme o controle

- Movimentação no menu: Joystick
- Seleção da opção no menu: Push botão que possui o led

### Design (2 pts)

<!--
Faca um esboco de como seria esse controle (vai ter uma etapa que terão que detalhar melhor isso).
-->

![alt text](https://github.com/insper-classroom/22b-emb-proj-embarcafunky/blob/master/ImgFinalControle.png?raw=true)

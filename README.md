# Tarefa de geração de animações em uma Matriz de led 5X5

Este projeto tem como objetivo o desenvolvimento de animações visuais em uma matriz de LEDs 5x5 RGB utilizando um Raspberry Pi Pico W e um teclado matricial 4x4. 
O sistema permitirá acionar diferentes animações de curta duração nos LEDs com base nas teclas pressionadas no teclado,
e o projeto também inclui o controle de cores e intensidades dos LEDs.

## Como Usar

Passo 1: Clone o repositório do projeto para o seu ambiente local

Passo 2: Antes de compilar o código, é necessário garantir que todas as extensões e ferramentas estão devidamente instaladas
Instalar o Pico SDK e extensão Raspberry Pi Pico: Siga as instruções do Pico SDK para configurar o ambiente de desenvolvimento no VS Code e instale a extensão Raspberry Pi Pico.
Instalar a Extensão Wokwi: Permite simular o Raspberry Pi Pico W no ambiente de desenvolvimento. Instale a extensão Wokwi Simulator no catálogo extensões do VS Code e depois habilite a mesma com uma licença  de 30 dias gratuita

Passo 3: Com o ambiente devidamente configurado, abra o código no VS Code e compile o projeto a partir do menu de compilação da extensão Raspberry Pi Pico.

Passo 4: Após a compilação, abra o arquivo diagram.json dentro do proprio VS Code localizado na pasta principal do projeto. Ao abrir o arquivo diagram.json, o Wokwi Simulator será automaticamente ativado e sera exibido a simulação já configurada para o projeto. 

Passo 5: Aperte o botão play no canto superior esquerdo e agora você pode interagir com o sistema diretamente no simulador e testar as funções.

## Funções

Teclass de 0 a 7: acionam animações específicas na matriz de LEDs 5x5.

Tecla A: Desliga todos os LEDs.

Tecla B: Liga todos os LEDs na cor azul, com 100% de luminosidade.

Tecla C: Liga todos os LEDs na cor vermelha, com 80% de luminosidade.

Tecla D: Liga todos os LEDs na cor verde, com 50% de luminosidade.

Tecla #: Liga todos os LEDs na cor branca, com 20% de luminosidade.

## Vídeo

Aqui está o vídeo demonstrando o funcionamento do sistema e das animações geradas com uma breve explicação das resoluções utilizadas.


## Equipe e suas funções na tarefa

Alex Rodrigo: Criação do repositório, gestão e revisão do projeto, README, implementação da função que reconhece o teclado e animação da tecla 2.

Maria Celiane: Diagrama para a simulação wokwi, e animação da tecla 6.

Péricles Alcântara: Animação da tecla 5.

Júlia Galindo: tecla D que liga todos os LEDs na cor verde em 50% de luminosidade, e animação da tecla 3.

Laerte Chaves: tecla A que desliga todos os LEDs, e animação da tecla 7.

Gabriel Oliveira: tecla # que liga todos os LEDs na cor Branca em 20% de luminosidade, e animação da tecla 4.

Jeová Cosme: tecla B que liga todos os LEDs na cor azul em 100% de luminosidade, e animação da tecla 1.

Gabriel Menezes: Revisão, tecla C que liga todos os LEDs na cor vermelha em 80% de luminosidade, e animação da tecla 0.

Todos os integrantes do grupo participaram ativamente na construção do código. No entanto, os integrantes Naiane Santos e Victor Henrick não participaram das atividades do projeto.

#include "pico/stdlib.h"
#include <stdio.h>
//BIBLIOTECAS PARA A MATRIZ LED
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/adc.h"
#include "pico/bootrom.h"

// Arquivo .pio
#include "pio_matrix.pio.h"

// Definições da matriz de LEDs
#define NUM_PIXELS 25
#define OUT_PIN 9
// Pino do buzzer
#define BUZZ 12

const uint8_t col[4] = {4, 3, 2, 1}; // Pinos das colunas do teclado
const uint8_t lin[4] = {8, 7, 6, 5}; // Pinos das linhas do teclado

const char tecl[4][4] = 
{
    {'1', '2', '3', 'A'}, 
    {'4', '5', '6', 'B'}, 
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

char ler_teclado();

// Vetores de imagens para a matriz de LEDs
double desenho1[25] = {
    0.0, 0.3, 0.0, 0.3, 0.0,
    0.0, 0.3, 0.3, 0.0, 0.3,
    0.3, 0.0, 0.3, 0.0, 0.0,
    0.3, 0.0, 0.0, 0.3, 0.0,
    0.0, 0.3, 0.3, 0.0, 0.3
    };

double desenho2[25] = {
    0.3, 0.0, 0.3, 0.0, 0.0,
    0.0, 0.3, 0.3, 0.3, 0.0,
    0.3, 0.0, 0.0, 0.3, 0.0,
    0.0, 0.0, 0.3, 0.0, 0.3,
    0.0, 0.3, 0.0, 0.3, 0.3
};

double desenho3[25] = {
    0.3, 0.0, 0.0, 0.3, 0.0,
    0.3, 0.0, 0.3, 0.0, 0.0,
    0.0, 0.3, 0.3, 0.3, 0.0,
    0.3, 0.3, 0.0, 0.3, 0.0,
    0.0, 0.0, 0.3, 0.0, 0.3
};

double desenho4[25] = {
    0.0, 0.0, 0.3, 0.0, 0.3,
    0.3, 0.3, 0.0, 0.3, 0.0,
    0.0, 0.0, 0.3, 0.3, 0.0,
    0.3, 0.3, 0.0, 0.0, 0.3,
    0.0, 0.3, 0.0, 0.3, 0.0
};

double desenho5[25] = {
    0.0, 0.3, 0.0, 0.0, 0.3,
    0.3, 0.0, 0.0, 0.3, 0.3,
    0.0, 0.3, 0.0, 0.3, 0.0,
    0.3, 0.0, 0.3, 0.0, 0.0,
    0.0, 0.3, 0.3, 0.0, 0.3
};


// Função para imprimir valor binário
void imprimir_binario(int num) {
    int i;
    for (i = 31; i >= 0; i--) {
        (num & (1 << i)) ? printf("1") : printf("0");
    }
}

// Função para configurar a matriz de LEDs
uint32_t matrix_rgb(double b, double r, double g) {
    unsigned char R, G, B;
    R = r * 255;
    G = g * 255;
    B = b * 255;
    return (G << 24) | (R << 16) | (B << 8);
}

// Função para acionar a matriz de LEDs
void desenho_pio(double *desenho, uint32_t valor_led, PIO pio, uint sm, double r, double g, double b) {
    for (int16_t i = 0; i < NUM_PIXELS; i++) {
        if (i % 2 == 0) {
            valor_led = matrix_rgb(desenho[24 - i], r = 0.0, g = 0.0);
            pio_sm_put_blocking(pio, sm, valor_led);
        } else {
            valor_led = matrix_rgb(b = 0.0, desenho[24 - i], g = 0.0);
            pio_sm_put_blocking(pio, sm, valor_led);
        }
    }
    imprimir_binario(valor_led);
}

int main() 
{
    // Inicializa comunicação serial
    stdio_init_all();

PIO pio = pio0;
    bool ok;
    uint16_t i;
    uint32_t valor_led;
    double r = 0.0, b = 0.0, g = 0.0;

    // Configura o clock para 128 MHz
    ok = set_sys_clock_khz(128000, false);
    printf("Iniciando a transmissão PIO\n");

    // Configurações do PIO
    uint offset = pio_add_program(pio, &pio_matrix_program);
    uint sm = pio_claim_unused_sm(pio, true);
    pio_matrix_program_init(pio, sm, offset, OUT_PIN);    

    // Configura buzzer como saída
    gpio_init(BUZZ);
    gpio_set_dir(BUZZ, GPIO_OUT);
    gpio_put(BUZZ, false);

    // Configura colunas como saídas digitais
    for (int i = 0; i < 4; i++) 
    {
        gpio_init(col[i]);
        gpio_set_dir(col[i], GPIO_OUT);
        gpio_put(col[i], 1); // Colunas iniciam em alto
    }

    // Configura linhas como entradas com pull-up
    for (int i = 0; i < 4; i++) 
    {
        gpio_init(lin[i]);
        gpio_set_dir(lin[i], GPIO_IN);
        gpio_pull_up(lin[i]); // Habilita pull-up nas linhas
    }

    while (true) 
    {
        char tecla = ler_teclado();

        if (tecla != 'n') // Só executa se uma tecla for pressionada
        {
            printf("Tecla pressionada: %c\n", tecla);

            // Ações para cada tecla
            switch (tecla) 
            {
                case '6':
                    desenho_pio(desenho1, valor_led, pio, sm, r, g, b); // Ação para o padrão 1
                    sleep_ms(500);
                    desenho_pio(desenho2, valor_led, pio, sm, r, g, b); // Ação para o padrão 2 
                    sleep_ms(500);                    
                    desenho_pio(desenho3, valor_led, pio, sm, r, g, b); // Ação para o padrão 3                   
                    sleep_ms(500);
                    desenho_pio(desenho4, valor_led, pio, sm, r, g, b); // Ação para o padrão 4                   
                    sleep_ms(500);
                    desenho_pio(desenho5, valor_led, pio, sm, r, g, b); // Ação para o padrão                    
                    sleep_ms(500);
                    break;

                default:
                    break;
            }
        }

        sleep_ms(200); // Pausa antes da próxima leitura
    }

    return 0;
}

// Função para ler o teclado
char ler_teclado() 
{
    char tecla = 'n'; // Valor padrão caso nenhuma tecla seja pressionada

    // Desliga todas as colunas
    for (int i = 0; i < 4; i++) 
    {
        gpio_put(col[i], 1);
    }

    for (int c = 0; c < 4; c++) 
    {
        // Ativa a coluna atual
        gpio_put(col[c], 0);

        for (int l = 0; l < 4; l++) 
        {
            // Verifica se a linha foi ativada
            if (gpio_get(lin[l]) == 0) 
            {
                tecla = tecl[l][c]; // Mapeia a tecla pressionada

                // Espera a tecla ser liberada (debounce)
                while (gpio_get(lin[l]) == 0) 
                {
                    sleep_ms(10);
                }
                break; // Sai do laço após detectar a tecla
            }
        }

        // Desativa a coluna atual
        gpio_put(col[c], 1);

        if (tecla != 'n') // Sai do laço de colunas se uma tecla for detectada
        {
            break;
        }
    }

    return tecla; // Retorna a tecla pressionada
}
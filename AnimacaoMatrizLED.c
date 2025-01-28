#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
//
// BIBLIOTECAS PARA A MATRIZ LED
#include <math.h>
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/adc.h"
#include "pico/bootrom.h"

// Arquivo .pio
#include "AnimacaoMatrizLED.pio.h"

// Definições da matriz de LEDs
#define NUM_PIXELS 25
#define OUT_PIN 7
// Pino do buzzer
#define BUZZ 12

// Função que ativa o buzzer por um curto periodo e depois desativa
void Ativar_buzzer(uint gpio, uint frequencia)
{
    uint slice_num = pwm_gpio_to_slice_num(gpio);
    gpio_set_function(gpio, GPIO_FUNC_PWM);

    uint32_t freq_relogio = 125000000;
    uint16_t contador = freq_relogio / frequencia;

    pwm_set_wrap(slice_num, contador);
    pwm_set_chan_level(slice_num, pwm_gpio_to_channel(gpio), contador / 2);

    pwm_set_enabled(slice_num, true);
    sleep_ms(50); // Som curto (50ms)
    pwm_set_enabled(slice_num, false);

    sleep_ms(500); // Pausa maior entre os sons para evitar efeito contínuo
}

// Pinos para o teclado matricial
const uint8_t teclas_colunas[4] = {4, 3, 2, 1};
const uint8_t teclas_linhas[4] = {9, 8, 6, 5};

// Mapeamento do teclado matricial
const char matriz_teclas[4][4] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}};

char capturar_tecla();


// Matrizes para as animações

// Vetores de imagens para a matriz de LEDs
double desenho1[25] = {
    0.0, 0.3, 0.0, 0.3, 0.0,
    0.0, 0.3, 0.3, 0.0, 0.3,
    0.3, 0.0, 0.3, 0.0, 0.0,
    0.3, 0.0, 0.0, 0.3, 0.0,
    0.0, 0.3, 0.3, 0.0, 0.3};

double desenho2[25] = {
    0.3, 0.0, 0.3, 0.0, 0.0,
    0.0, 0.3, 0.3, 0.3, 0.0,
    0.3, 0.0, 0.0, 0.3, 0.0,
    0.0, 0.0, 0.3, 0.0, 0.3,
    0.0, 0.3, 0.0, 0.3, 0.3};

double desenho3[25] = {
    0.3, 0.0, 0.0, 0.3, 0.0,
    0.3, 0.0, 0.3, 0.0, 0.0,
    0.0, 0.3, 0.3, 0.3, 0.0,
    0.3, 0.3, 0.0, 0.3, 0.0,
    0.0, 0.0, 0.3, 0.0, 0.3};

double desenho4[25] = {
    0.0, 0.0, 0.3, 0.0, 0.3,
    0.3, 0.3, 0.0, 0.3, 0.0,
    0.0, 0.0, 0.3, 0.3, 0.0,
    0.3, 0.3, 0.0, 0.0, 0.3,
    0.0, 0.3, 0.0, 0.3, 0.0};

double desenho5[25] = {
    0.0, 0.3, 0.0, 0.0, 0.3,
    0.3, 0.0, 0.0, 0.3, 0.3,
    0.0, 0.3, 0.0, 0.3, 0.0,
    0.3, 0.0, 0.3, 0.0, 0.0,
    0.0, 0.3, 0.3, 0.0, 0.3};

// Desenhos para a animação da tecla '3'
double frame1[25] = {
    0.2, 0.0, 0.2, 0.0, 0.2,
    0.0, 0.2, 0.0, 0.2, 0.0,
    0.2, 0.0, 0.2, 0.0, 0.2,
    0.0, 0.2, 0.0, 0.2, 0.0,
    0.2, 0.0, 0.2, 0.0, 0.2};

double frame2[25] = {
    0.0, 0.2, 0.0, 0.2, 0.0,
    0.2, 0.0, 0.2, 0.0, 0.2,
    0.0, 0.2, 0.0, 0.2, 0.0,
    0.2, 0.0, 0.2, 0.0, 0.2,
    0.0, 0.2, 0.0, 0.2, 0.0};

double frame3[25] = {
    0.2, 0.2, 0.2, 0.2, 0.2,
    0.2, 0.0, 0.0, 0.0, 0.2,
    0.2, 0.0, 0.2, 0.0, 0.2,
    0.2, 0.0, 0.0, 0.0, 0.2,
    0.2, 0.2, 0.2, 0.2, 0.2};

double frame4[25] = {
    0.0, 0.0, 0.2, 0.0, 0.0,
    0.0, 0.2, 0.0, 0.2, 0.0,
    0.2, 0.0, 0.0, 0.0, 0.2,
    0.0, 0.2, 0.0, 0.2, 0.0,
    0.0, 0.0, 0.2, 0.0, 0.0};

double frame5[25] = {
    0.2, 0.2, 0.0, 0.2, 0.2,
    0.2, 0.0, 0.2, 0.0, 0.2,
    0.0, 0.2, 0.0, 0.2, 0.0,
    0.2, 0.0, 0.2, 0.0, 0.2,
    0.2, 0.2, 0.0, 0.2, 0.2};

// Desenhos para a animação da tecla '1'
// Frame 1
double cruzAlternada_frame1[25] = {
    0.3, 0.0, 0.0, 0.0, 0.3,
    0.0, 0.3, 0.0, 0.3, 0.0,
    0.0, 0.0, 0.3, 0.0, 0.0,
    0.0, 0.3, 0.0, 0.3, 0.0,
    0.3, 0.0, 0.0, 0.0, 0.3};

// Frame 2
double cruzAlternada_frame2[25] = {
    0.0, 0.3, 0.0, 0.3, 0.0,
    0.3, 0.0, 0.3, 0.0, 0.3,
    0.0, 0.3, 0.0, 0.3, 0.0,
    0.3, 0.0, 0.3, 0.0, 0.3,
    0.0, 0.3, 0.0, 0.3, 0.0};

// Frame 3
double cruzAlternada_frame3[25] = {
    0.3, 0.3, 0.0, 0.3, 0.3,
    0.3, 0.0, 0.3, 0.0, 0.3,
    0.0, 0.3, 0.0, 0.3, 0.0,
    0.3, 0.0, 0.3, 0.0, 0.3,
    0.3, 0.3, 0.0, 0.3, 0.3};

// Frame 4
double cruzAlternada_frame4[25] = {
    0.0, 0.3, 0.3, 0.3, 0.0,
    0.3, 0.0, 0.0, 0.0, 0.3,
    0.3, 0.0, 0.3, 0.0, 0.3,
    0.3, 0.0, 0.0, 0.0, 0.3,
    0.0, 0.3, 0.3, 0.3, 0.0};

// Frame 5
double cruzAlternada_frame5[25] = {
    0.3, 0.0, 0.0, 0.0, 0.3,
    0.0, 0.3, 0.0, 0.3, 0.0,
    0.0, 0.0, 0.3, 0.0, 0.0,
    0.0, 0.3, 0.0, 0.3, 0.0,
    0.3, 0.0, 0.0, 0.0, 0.3};

// Desenhos para a animação da tecla '4'
// Frame 1
double onda1[25] = {
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.4, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0};

// Frame 2
double onda2[25] = {
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.4, 0.4, 0.4, 0.0,
    0.0, 0.4, 0.0, 0.4, 0.0,
    0.0, 0.4, 0.4, 0.4, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0};

// Frame 3
double onda3[25] = {
    0.4, 0.4, 0.4, 0.4, 0.4,
    0.4, 0.0, 0.0, 0.0, 0.4,
    0.4, 0.0, 0.0, 0.0, 0.4,
    0.4, 0.0, 0.0, 0.0, 0.4,
    0.4, 0.4, 0.4, 0.4, 0.4};

// Frame 4
double onda4[25] = {
    0.4, 0.0, 0.0, 0.0, 0.4,
    0.0, 0.4, 0.4, 0.4, 0.0,
    0.0, 0.4, 0.0, 0.4, 0.0,
    0.0, 0.4, 0.4, 0.4, 0.0,
    0.4, 0.0, 0.0, 0.0, 0.4};

// Frame 5
double onda5[25] = {
    0.4, 0.0, 0.0, 0.0, 0.4,
    0.0, 0.4, 0.0, 0.4, 0.0,
    0.0, 0.0, 0.4, 0.0, 0.0,
    0.0, 0.4, 0.0, 0.4, 0.0,
    0.4, 0.0, 0.0, 0.0, 0.4};

// Desenhos para a animação da tecla '5'
// Frame 1
double img1[25] = {
    1.0, 1.0, 0.0, 0.0, 0.0,
    1.0, 1.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0};

// Frame 2
double img2[25] = {
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.333, 0.333, 0.333, 0.0,
    0.0, 0.333, 0.333, 0.333, 0.0,
    0.0, 0.333, 0.333, 0.333, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0};

// Frame 3
double img3[25] = {
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.333, 0.333,
    0.0, 0.0, 0.0, 0.333, 0.333};

// Frame 4
double img4[25] = {
    1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 0.0, 0.0, 0.0, 1.0,
    1.0, 0.0, 0.0, 0.0, 1.0,
    1.0, 0.0, 0.0, 0.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0};

// Frame 5
double img5[25] = {
    1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 0.333, 0.333, 0.333, 1.0,
    1.0, 0.333, 0.0, 0.333, 1.0,
    1.0, 0.333, 0.333, 0.333, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0};

// Amimação da tecla 07
double explosao[25] = {
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.3, 0.0, 0.0,
    0.0, 0.3, 0.3, 0.3, 0.0,
    0.0, 0.0, 0.3, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0};

double explosao2[25] = {
    0.0, 0.0, 0.3, 0.0, 0.0,
    0.0, 0.2, 0.0, 0.2, 0.0,
    0.3, 0.0, 0.0, 0.0, 0.3,
    0.0, 0.2, 0.0, 0.2, 0.0,
    0.0, 0.0, 0.3, 0.0, 0.0};

double explosao3[25] = {
    0.1, 0.0, 0.0, 0.0, 0.1,
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.2, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.1, 0.0, 0.0, 0.0, 0.1};

double explosao4[25] = {
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.2, 0.0, 0.0,
    0.0, 0.2, 0.0, 0.2, 0.0,
    0.0, 0.0, 0.2, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0};

double explosao5[25] = {
    0.0, 0.0, 0.1, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.1, 0.0, 0.0, 0.0, 0.1,
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.1, 0.0, 0.0};

// frames animação de coração - Gabriel Menezes
// Frames para a animação do coração (10 frames)
double coracao_frames[15][25] = {
    // Frame 1
    {
        0.0, 0.0, 1.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0, 0.0},
    // Frame 2
    {
        0.0, 0.0, 1.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 1.0, 0.0,
        0.0, 0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0, 0.0},
    // Frame 3
    {
        0.0, 0.0, 1.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 1.0, 0.0,
        1.0, 0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0, 0.0},
    // Frame 4
    {
        0.0, 0.0, 1.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 1.0, 0.0,
        1.0, 0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0, 1.0,
        0.0, 0.0, 0.0, 0.0, 0.0},
    // Frame 5
    {
        0.0, 0.0, 1.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 1.0, 0.0,
        1.0, 0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0, 1.0,
        0.0, 1.0, 0.0, 0.0, 0.0},
    // Frame 6
    {
        0.0, 0.0, 1.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 1.0, 0.0,
        1.0, 0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0, 1.0,
        0.0, 1.0, 0.0, 0.0, 0.0},
    // Frame 7
    {
        0.0, 0.0, 1.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 1.0, 0.0,
        1.0, 0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0, 1.0,
        0.0, 1.0, 0.0, 1.0, 0.0},
    // Frame 8
    {
        0.0, 0.0, 1.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 1.0, 0.0,
        1.0, 0.0, 0.0, 0.0, 0.0,
        1.0, 0.0, 1.0, 0.0, 1.0,
        0.0, 1.0, 0.0, 1.0, 0.0},
    // Frame 9
    {
        0.0, 0.0, 1.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 1.0, 0.0,
        1.0, 0.0, 0.0, 0.0, 1.0,
        1.0, 0.0, 1.0, 0.0, 1.0,
        0.0, 1.0, 0.0, 1.0, 0.0},
    // Frame 10
    {
        0.0, 0.0, 1.0, 0.0, 0.0,
        0.0, 1.0, 0.0, 1.0, 0.0,
        1.0, 0.0, 0.0, 0.0, 1.0,
        1.0, 0.0, 1.0, 0.0, 1.0,
        0.0, 1.0, 0.0, 1.0, 0.0},
    // Frame 11
    {
        0.0, 0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0, 0.0},
    // Frame 12
    {
        0.0, 0.0, 1.0, 0.0, 0.0,
        0.0, 1.0, 0.0, 1.0, 0.0,
        1.0, 0.0, 0.0, 0.0, 1.0,
        1.0, 0.0, 1.0, 0.0, 1.0,
        0.0, 1.0, 0.0, 1.0, 0.0},
    // Frame 13
    {
        0.0, 0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0, 0.0},
    // Frame 14
    {
        0.0, 0.0, 1.0, 0.0, 0.0,
        0.0, 1.0, 0.0, 1.0, 0.0,
        1.0, 0.0, 0.0, 0.0, 1.0,
        1.0, 0.0, 1.0, 0.0, 1.0,
        0.0, 1.0, 0.0, 1.0, 0.0},
    // Frame 15
    {
        0.0, 0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0, 0.0}

};

//Matriz de frames da animação cobrinha - Alex Rodrigo 
double cobrinha[16][25] = {
    {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}, // Frame 1
    {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.9, 0.0}, // Frame 2
    {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.9, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.1, 0.0}, // Frame 3
    {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.9, 0.0, 0.0, 0.1, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}, // Frame 4
    {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.9, 0.1, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}, // Frame 5
    {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.9, 0.1, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}, // Frame 6
    {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.1, 0.1, 0.0, 0.0, 0.0, 0.0, 0.0, 0.9, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}, // Frame 7
    {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.1, 0.0, 0.0, 0.0, 0.0, 0.0, 0.9, 0.1, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}, // Frame 8
    {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.9, 0.1, 0.1, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}, // Frame 9
    {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.9, 0.0, 0.0, 0.1, 0.1, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}, // Frame 10
    {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.9, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.1, 0.0, 0.0, 0.1, 0.1, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}, // Frame 11
    {0.0, 0.0, 0.0, 0.9, 0.0, 0.0, 0.1, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.1, 0.0, 0.0, 0.1, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}, // Frame 12
    {0.0, 0.0, 0.0, 0.1, 0.0, 0.0, 0.1, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.1, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}, // Frame 13
    {0.0, 0.0, 0.0, 0.1, 0.0, 0.0, 0.1, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}, // Frame 14
    {0.0, 0.0, 0.0, 0.1, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}, // Frame 15
    {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}

}; // 1    2    3    4    5    6    7    8    9    10   11   12   13   14   15   16   17   18   19   20   21   22   23   24   25



// Função para imprimir valor binário
void imprimir_binario(int num)
{
    int i;
    for (i = 31; i >= 0; i--)
    {
        (num & (1 << i)) ? printf("1") : printf("0");
    }
}

// Função para configurar a matriz de LEDs
uint32_t matrix_rgb(double b, double r, double g)
{
    unsigned char R, G, B;
    R = r * 255;
    G = g * 255;
    B = b * 255;
    return (G << 24) | (R << 16) | (B << 8);
}

// Função para acionar a matriz de LEDs
void desenho_pio(double *desenho, uint32_t valor_led, PIO pio, uint sm, double r, double g, double b)
{
    for (int16_t i = 0; i < NUM_PIXELS; i++)
    {
        if (i % 2 == 0)
        {
            valor_led = matrix_rgb(desenho[24 - i], r = 0.0, g = 0.0);
            pio_sm_put_blocking(pio, sm, valor_led);
        }
        else
        {
            valor_led = matrix_rgb(b = 0.0, desenho[24 - i], g = 0.0);
            pio_sm_put_blocking(pio, sm, valor_led);
        }
    }
    imprimir_binario(valor_led);
}

// Função para ligar todos os LEDs em uma sequência de frames (Animação)
// **Nova Função para Exibir a Animação do Coração**
void exibir_coracao(PIO pio, uint sm, double coracao[15][25])
{
    uint32_t valor_led;

    for (int frame = 0; frame < 15; frame++) // Itera pelos 15 frames
    {
        for (int i = 0; i < NUM_PIXELS; i++) // Itera pelos LEDs
        {
            double brilho = coracao[frame][i];
            valor_led = matrix_rgb(0.0, brilho, 0.0); // Cor vermelha

            // Envia o valor calculado ao LED
            pio_sm_put_blocking(pio, sm, valor_led);
        }
        imprimir_binario(valor_led);
        sleep_ms(300); // Aguarda 300ms antes de mudar para o próximo frame
    }
}

// Função que anima a cobrinha, nescessária pois utiliza a cor verde e vermelha ao mesmo tempo
void exibir_cobrinha(PIO pio, uint sm, double cobrinha[16][25])
{
    uint32_t valor_led;

    for (int frame = 0; frame < 16; frame++)
    { // Itera pelos 16 frames
        for (int i = 0; i < NUM_PIXELS; i++)
        { // Itera pelos LEDs
            double brilho = cobrinha[frame][i];

            // Configuração de cores: vermelho para intensidade > 0.9, verde para <= 0.9
            if (brilho > 0.9)
            {
                valor_led = matrix_rgb(0.0, brilho, 0.0); // Cor vermelha
            }
            else
            {
                valor_led = matrix_rgb(0.0, 0.0, brilho); // Cor verde
            }

            // Envia o valor calculado ao LED
            pio_sm_put_blocking(pio, sm, valor_led);
        }
        sleep_ms(700); // Aguarda 200ms antes de mudar para o próximo frame
    }
}

// Função para ligar todos os LEDs no vermelho com 80% de intensidade
void ligar_leds_vermelho(PIO pio, uint sm)
{
    uint32_t valor_led = matrix_rgb(0.0, 0.8, 0.0); // 80% de intensidade no vermelho
    for (int i = 0; i < NUM_PIXELS; i++)
    {
        pio_sm_put_blocking(pio, sm, valor_led);
    }
}

// Função para ligar todos os LEDs na cor verde com 50% de intensidade
void ligar_leds_verde(PIO pio, uint sm)
{
    uint32_t valor_led = matrix_rgb(0.0, 0.0, 0.5); // 50% de intensidade no verde
    for (int i = 0; i < NUM_PIXELS; i++)
    {
        pio_sm_put_blocking(pio, sm, valor_led);
    }
}

// Função para ligar todos os LEDs na cor azul com 100% de intensidade
void ligar_leds_azul(PIO pio, uint sm)
{
    uint32_t valor_led = matrix_rgb(1.0, 0.0, 0.0); // 100% de intensidade no azul
    for (int i = 0; i < NUM_PIXELS; i++)
    {
        pio_sm_put_blocking(pio, sm, valor_led);
    }
}

// Função para ligar todos os LEDs na branca com 20% de intensidade
void ligar_leds_branco(PIO pio, uint sm)
{
    uint32_t valor_led = matrix_rgb(0.2, 0.2, 0.2); // 100% de intensidade no azul
    for (int i = 0; i < NUM_PIXELS; i++)
    {
        pio_sm_put_blocking(pio, sm, valor_led);
    }
}

// Função para desligar todos os LEDs
void desligar_leds(PIO pio, uint sm)
{
    uint32_t valor_led = matrix_rgb(0.0, 0.0, 0.0); // Nenhuma intensidade (LEDs apagados)
    for (int i = 0; i < NUM_PIXELS; i++)
    {
        pio_sm_put_blocking(pio, sm, valor_led); // Envia o valor para cada LED
    }
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
    uint offset = pio_add_program(pio, &AnimacaoMatrizLED_program);
    uint sm = pio_claim_unused_sm(pio, true);
    AnimacaoMatrizLED_program_init(pio, sm, offset, OUT_PIN);

    // Configura buzzer como saída
    gpio_init(BUZZ);
    gpio_set_dir(BUZZ, GPIO_OUT);
    gpio_put(BUZZ, false);

    // Configura colunas como saídas digitais
    for (int i = 0; i < 4; i++)
    {
        gpio_init(teclas_colunas[i]);
        gpio_set_dir(teclas_colunas[i], GPIO_OUT);
        gpio_put(teclas_colunas[i], 1); // Colunas iniciam em alto
    }

    // Configura linhas como entradas com pull-up
    for (int i = 0; i < 4; i++)
    {
        gpio_init(teclas_linhas[i]);
        gpio_set_dir(teclas_linhas[i], GPIO_IN);
        gpio_pull_up(teclas_linhas[i]); // Habilita pull-up nas linhas
    }

    while (true)
    {
        char tecla = capturar_tecla();

        if (tecla != 'n') // Só executa se uma tecla for pressionada
        {
            printf("Tecla pressionada: %c\n", tecla);

            // Ações para cada tecla
            switch (tecla)
            {
            case '0':
                exibir_coracao(pio, sm, coracao_frames);
                break;
            case '1':
                desenho_pio(cruzAlternada_frame1, valor_led, pio, sm, r, g, b); // Frame 1
                sleep_ms(500);
                desenho_pio(cruzAlternada_frame2, valor_led, pio, sm, r, g, b); // Frame 2
                sleep_ms(500);
                desenho_pio(cruzAlternada_frame3, valor_led, pio, sm, r, g, b); // Frame 3
                sleep_ms(500);
                desenho_pio(cruzAlternada_frame4, valor_led, pio, sm, r, g, b); // Frame 4
                sleep_ms(500);
                desenho_pio(cruzAlternada_frame5, valor_led, pio, sm, r, g, b); // Frame 5
                sleep_ms(500);
                break;
            case '2':
                exibir_cobrinha(pio, sm, cobrinha);
                break;
            case '3':
                desenho_pio(frame1, valor_led, pio, sm, r, g, b); // Frame 1
                sleep_ms(500);
                desenho_pio(frame2, valor_led, pio, sm, r, g, b); // Frame 2
                sleep_ms(500);
                desenho_pio(frame3, valor_led, pio, sm, r, g, b); // Frame 3
                sleep_ms(500);
                desenho_pio(frame4, valor_led, pio, sm, r, g, b); // Frame 4
                sleep_ms(500);
                desenho_pio(frame5, valor_led, pio, sm, r, g, b); // Frame 5
                sleep_ms(500);
                break;

            case '4':
                Ativar_buzzer(BUZZ, 500);
                desenho_pio(onda1, valor_led, pio, sm, r, g, b); // Frame 1
                sleep_ms(500);
                Ativar_buzzer(BUZZ, 600);
                desenho_pio(onda2, valor_led, pio, sm, r, g, b); // Frame 2
                sleep_ms(500);
                Ativar_buzzer(BUZZ, 700);
                desenho_pio(onda3, valor_led, pio, sm, r, g, b); // Frame 3
                sleep_ms(500);
                Ativar_buzzer(BUZZ, 800);
                desenho_pio(onda4, valor_led, pio, sm, r, g, b); // Frame 4
                sleep_ms(500);
                Ativar_buzzer(BUZZ, 1000);
                desenho_pio(onda5, valor_led, pio, sm, r, g, b); // Frame 5
                sleep_ms(500);
                break;

            case '5':
                Ativar_buzzer(BUZZ, 500);
                desenho_pio(img1, valor_led, pio, sm, r, g, b); // Frame 1
                sleep_ms(500);
                Ativar_buzzer(BUZZ, 600);
                desenho_pio(img2, valor_led, pio, sm, r, g, b); // Frame 2
                sleep_ms(500);
                Ativar_buzzer(BUZZ, 700);
                desenho_pio(img3, valor_led, pio, sm, r, g, b); // Frame 3
                sleep_ms(500);
                Ativar_buzzer(BUZZ, 800);
                desenho_pio(img4, valor_led, pio, sm, r, g, b); // Frame 4
                sleep_ms(500);
                Ativar_buzzer(BUZZ, 1000);
                desenho_pio(img5, valor_led, pio, sm, r, g, b); // Frame 5
                sleep_ms(500);
                break;

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

            case '7':
                desenho_pio(explosao, valor_led, pio, sm, r, g, b); // Frame 1
                sleep_ms(500);
                desenho_pio(explosao2, valor_led, pio, sm, r, g, b); // Frame 1
                sleep_ms(500);
                desenho_pio(explosao3, valor_led, pio, sm, r, g, b); // Frame 1
                sleep_ms(500);
                desenho_pio(explosao4, valor_led, pio, sm, r, g, b); // Frame 1
                sleep_ms(500);
                desenho_pio(explosao5, valor_led, pio, sm, r, g, b); // Frame 1
                sleep_ms(500);
                break;

            case 'A':
                desligar_leds(pio, sm); // Desliga todos os LEDs
                break;

            case 'B':
                ligar_leds_azul(pio, sm); // Liga todos os LEDs na cor azul com 100% de intensidade
                sleep_ms(500);
                break;
            case 'C':
                ligar_leds_vermelho(pio, sm); // Liga todos os LEDs no vermelho com 80% de intensidade
                sleep_ms(500);
                break;
            case 'D':
                ligar_leds_verde(pio, sm); // Liga todos os LEDs na cor verde com 50% de intensidade
                sleep_ms(500);
                break;

            case '#':
                ligar_leds_branco(pio, sm); // Liga todos os LEDs na cor branca com 20% de intensidade
                sleep_ms(500);

            default:
                break;
            }
        }

        sleep_ms(200); // Pausa antes da próxima leitura
    }

    return 0;
}

// Função para identificar a tecla pressionada
char capturar_tecla()
{
    // Reseta todas as colunas
    for (int i = 0; i < 4; i++)
    {
        gpio_put(teclas_colunas[i], 1);
    }

    for (int coluna = 0; coluna < 4; coluna++)
    {
        gpio_put(teclas_colunas[coluna], 0); // Ativa a coluna atual

        for (int linha = 0; linha < 4; linha++)
        {
            if (gpio_get(teclas_linhas[linha]) == 0)
            {                                        // Detecta pressão na linha
                gpio_put(teclas_colunas[coluna], 1); // Desativa a coluna atual
                return matriz_teclas[linha][coluna]; // Retorna imediatamente
            }
        }

        gpio_put(teclas_colunas[coluna], 1); // Desativa a coluna atual
    }

    return 'n'; // Nenhuma tecla pressionada
}

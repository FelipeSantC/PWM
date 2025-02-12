#include "pico/stdlib.h"
#include "hardware/pwm.h"

// Definindo os pinos e frequências
#define PWM_PIN 22 // GPIO 22
#define PWM_FREQUENCY 50 // Frequência de 50Hz
#define PERIOD_MS 20 // Período de 20ms (50Hz)

#define ANGLE_180_DUTY 2400  // Ciclo ativo para 180 graus (2400us)
#define ANGLE_90_DUTY 1470   // Ciclo ativo para 90 graus (1470us)
#define ANGLE_0_DUTY 500     // Ciclo ativo para 0 graus (500us)

void setup_pwm() {
    // Configura o pino PWM
    gpio_set_function(PWM_PIN, GPIO_FUNC_PWM);

    // Pega o número do canal de PWM associado ao pino
    uint slice_num = pwm_gpio_to_slice_num(PWM_PIN);

    // Configura a frequência de PWM para 50Hz
    pwm_set_clkdiv(slice_num, 125.f); // Divisor de clock para 50Hz (período de 20ms)

    // Configura a largura do pulso inicial para 1.470us (posição de 90 graus)
    pwm_set_gpio_level(PWM_PIN, ANGLE_90_DUTY); // Ciclo de trabalho para 90 graus

    // Ativa o PWM
    pwm_set_enabled(slice_num, true);
}

void move_servo(uint pulse_width) {
    // Move o servo para o ciclo de trabalho (largura do pulso) fornecido em microssegundos
    uint slice_num = pwm_gpio_to_slice_num(PWM_PIN);
    pwm_set_gpio_level(PWM_PIN, pulse_width);
}

void smooth_move_servo(int start, int end, int step, int delay_ms) {
    // Movimenta o servo suavemente entre os valores de 'start' e 'end' com um 'step' especificado.
    if (start < end) {
        for (int duty = start; duty <= end; duty += step) {
            move_servo(duty);
            sleep_us(delay_ms); // Atraso em microssegundos
        }
    } else {
        for (int duty = start; duty >= end; duty -= step) {
            move_servo(duty);
            sleep_us(delay_ms); // Atraso em microssegundos
        }
    }
}   
void wait_for_seconds(int seconds) {
    sleep_ms(seconds * 1000); // Espera pelo tempo especificado em segundos
}

int main() {
    stdio_init_all(); // Inicializa a comunicação serial
    setup_pwm(); // Configura o PWM para o controle do servo

    wait_for_seconds(2); // Aguarda 2 segundos

    // Movimentar o servomotor para 180 graus (2400us)
    move_servo(ANGLE_180_DUTY);
    wait_for_seconds(5); // Aguarda 5 segundos

    // Movimentar o servomotor para 90 graus (1470us)
    move_servo(ANGLE_90_DUTY);
    wait_for_seconds(5); // Aguarda 5 segundos

    // Movimentar o servomotor para 0 graus (500us)
    move_servo(ANGLE_0_DUTY);
    wait_for_seconds(5); // Aguarda 5 segundos

    // Movimentação periódica suave entre 0 e 180 graus
    while (true) {
        // Movimentar de 0 para 180 graus suavemente
        smooth_move_servo(ANGLE_0_DUTY, ANGLE_180_DUTY, 5, 10000); // Atraso de 1000 microssegundos (1ms)

        // Movimentar de 180 para 0 graus suavemente
        smooth_move_servo(ANGLE_180_DUTY, ANGLE_0_DUTY, 5, 10000); // Atraso de 1000 microssegundos (1ms)
    }

    return 0;
}

# Controle de Servomotor com PWM - Projeto para RP2040 (Pico SDK)

Este projeto utiliza o microcontrolador RP2040 (Raspberry Pi Pico) para controlar a posição de um servomotor padrão usando a modulação por largura de pulso (PWM). O código foi desenvolvido utilizando o Pico SDK e foi projetado para simular o movimento do servomotor entre diferentes ângulos (0°, 90° e 180°), além de permitir um movimento suave entre essas posições.

### Requisitos

- **Hardware**: Microcontrolador RP2040 (Raspberry Pi Pico)
- **Software**: Pico SDK, CMake
- **Bibliotecas necessárias**:
  - `pico/stdlib.h` (biblioteca padrão do Pico SDK)
  - `hardware/pwm.h` (biblioteca para controle do PWM)

### Funcionalidades

1. **Controle de posição do servomotor**:
   - O código usa a técnica de **modulação por largura de pulso (PWM)** para controlar a posição do servomotor.
   - A posição do servomotor é definida ajustando o ciclo de trabalho do PWM em diferentes valores, o que corresponde a diferentes ângulos.

2. **Posições de 0°, 90° e 180°**:
   - O servomotor é movido para três posições: 0°, 90° e 180°.
   - O ciclo de trabalho PWM correspondente para cada posição:
     - **0 graus**: 500µs (0,025% de ciclo ativo)
     - **90 graus**: 1470µs (0,0735% de ciclo ativo)
     - **180 graus**: 2400µs (0,12% de ciclo ativo)

3. **Movimentação suave do servomotor**:
   - A movimentação entre 0° e 180° é feita de forma suave, utilizando incrementos no ciclo de trabalho PWM, com um atraso de 10ms (10000 microssegundos) entre cada ajuste.

### Descrição do Código

#### 1. **Definições de Pinos e Frequência PWM**
```c
#define PWM_PIN 22 // GPIO 22
#define PWM_FREQUENCY 50 // Frequência de 50Hz (Período de 20ms)
#define PERIOD_MS 20 // Período de 20ms (50Hz)

#define ANGLE_180_DUTY 2400  // Ciclo ativo para 180 graus (2400us)
#define ANGLE_90_DUTY 1470   // Ciclo ativo para 90 graus (1470us)
#define ANGLE_0_DUTY 500     // Ciclo ativo para 0 graus (500us)
```
Aqui, configuramos o pino GPIO 22 para controle do PWM, com uma frequência de 50Hz (que resulta em um período de 20ms). Definimos os valores dos ciclos de trabalho (duty cycles) para as três posições do servomotor: 0°, 90° e 180°.

#### 2. **Função `setup_pwm()`**
```c
void setup_pwm() {
    gpio_set_function(PWM_PIN, GPIO_FUNC_PWM); // Configura o pino para PWM
    uint slice_num = pwm_gpio_to_slice_num(PWM_PIN); // Obtém o número do canal PWM
    pwm_set_clkdiv(slice_num, 125.f); // Define o divisor de clock para gerar 50Hz
    pwm_set_gpio_level(PWM_PIN, ANGLE_90_DUTY); // Define o ciclo de trabalho para 90 graus
    pwm_set_enabled(slice_num, true); // Ativa o PWM
}
```
Essa função configura o pino GPIO para trabalhar com PWM, ajusta a frequência de PWM para 50Hz e define um ciclo de trabalho inicial de 90 graus.

#### 3. **Função `move_servo(uint pulse_width)`**
```c
void move_servo(uint pulse_width) {
    uint slice_num = pwm_gpio_to_slice_num(PWM_PIN); // Obtém o número do canal PWM
    pwm_set_gpio_level(PWM_PIN, pulse_width); // Define o ciclo de trabalho para a largura de pulso fornecida
}
```
A função `move_servo` ajusta o ciclo de trabalho (duty cycle) do PWM para mover o servomotor para a posição desejada, com base na largura do pulso fornecida (em microssegundos).

#### 4. **Função `smooth_move_servo(int start, int end, int step, int delay_ms)`**
```c
void smooth_move_servo(int start, int end, int step, int delay_ms) {
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
```
Essa função realiza o movimento suave do servomotor entre os valores `start` e `end`, ajustando o ciclo de trabalho em incrementos ou decrementos de `step` a cada intervalo de `delay_ms` microssegundos. Isso garante uma transição suave entre as posições.

#### 5. **Função `wait_for_seconds(int seconds)`**
```c
void wait_for_seconds(int seconds) {
    sleep_ms(seconds * 1000); // Aguarda o tempo especificado em segundos
}
```
A função `wait_for_seconds` simplesmente cria uma pausa de `seconds` segundos, utilizando a função `sleep_ms` do SDK para gerar o atraso.

#### 6. **Função `main()`**
```c
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
        smooth_move_servo(ANGLE_0_DUTY, ANGLE_180_DUTY, 5, 10000); // Atraso de 1000 microssegundos (1ms)
        smooth_move_servo(ANGLE_180_DUTY, ANGLE_0_DUTY, 5, 10000); // Atraso de 1000 microssegundos (1ms)
    }

    return 0;
}
```
No `main()`, o código executa os seguintes passos:
1. Inicializa a comunicação serial e o PWM.
2. Realiza a movimentação do servomotor para 180°, 90° e 0° com um tempo de espera entre cada movimento.
3. Entra em um loop infinito para realizar movimentações suaves periódicas entre 0° e 180°, repetidamente.

### Considerações Finais

- **Atrasos e Incrementos**: O movimento do servomotor pode ser ajustado modificando o valor de `step` na função `smooth_move_servo()` ou o atraso entre os ajustes (`delay_ms`), dependendo da suavidade ou velocidade desejada.
- **Precisão**: A precisão do controle depende diretamente da configuração de `sleep_us()` e dos incrementos no duty cycle.
  
Este projeto é útil para aplicações que necessitam de um controle preciso de servomotores, como em robótica, sistemas de controle de câmera ou mecanismos automatizados.

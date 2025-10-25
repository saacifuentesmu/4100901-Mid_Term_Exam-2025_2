#include "room_control.h"

#include "gpio.h"    // Para controlar LEDs
#include "systick.h" // Para obtener ticks y manejar tiempos
#include "uart.h"    // Para enviar mensajes
#include "tim.h"     // Para controlar el PWM

// Estados de la sala
typedef enum {
    ROOM_IDLE,
    ROOM_OCCUPIED
} room_state_t;

// Variable de estado global
room_state_t current_state = ROOM_IDLE;
static uint32_t led_on_time = 0;
static uint8_t current_duty_cycle = PWM_INITIAL_DUTY;  // Variable global para el duty cycle actual


static void uart_send_number(uint8_t number);

void room_control_app_init(void)
{
    // Inicializar PWM al duty cycle inicial (estado IDLE -> LED apagado)
    tim3_ch1_pwm_set_duty_cycle(PWM_INITIAL_DUTY);

    // Enviar mensaje de bienvenida personalizado
    uart_send_string("Controlador de Sala v2.0\r\n");
    uart_send_string("Estado inicial:\r\n");
    uart_send_string(" - Lampara: 20%\r\n");
    uart_send_string(" - Puerta: Cerrada\r\n");
}

void room_control_on_button_press(void)
{
    if (current_state == ROOM_IDLE) {
        current_state = ROOM_OCCUPIED;
        current_duty_cycle = 100;  // Actualizar variable global
        tim3_ch1_pwm_set_duty_cycle(100);  // PWM al 100%
        led_on_time = systick_get_ms();
        uart_send_string("Sala ocupada\r\n");
    } else {
        current_state = ROOM_IDLE;
        current_duty_cycle = 0;  // Actualizar variable global
        tim3_ch1_pwm_set_duty_cycle(0);  // PWM al 0%
        uart_send_string("Sala vacía\r\n");
    }
}

void room_control_on_uart_receive(char received_char)
{
    switch (received_char) {
        case 'h':
        case 'H':
            tim3_ch1_pwm_set_duty_cycle(100);
            current_duty_cycle = 100;  // Actualizar variable global
            uart_send_string("PWM: 100%\r\n");
            break;
        case 'l':
        case 'L':
            tim3_ch1_pwm_set_duty_cycle(0);
            current_duty_cycle = 0;  // Actualizar variable global
            uart_send_string("PWM: 0%\r\n");
            break;
        case 'O':
        case 'o':
            current_state = ROOM_OCCUPIED;
            tim3_ch1_pwm_set_duty_cycle(100);
            current_duty_cycle = 100;  // Actualizar variable global
            led_on_time = systick_get_ms();
            uart_send_string("Sala ocupada\r\n");
            break;
        case 'I':
        case 'i':
            current_state = ROOM_IDLE;
            tim3_ch1_pwm_set_duty_cycle(0);
            current_duty_cycle = 0;  // Actualizar variable global
            uart_send_string("Sala vacía\r\n");
            break;
        case '1':
            tim3_ch1_pwm_set_duty_cycle(10);
            current_duty_cycle = 10;  // Actualizar variable global
            uart_send_string("PWM: 10%\r\n");
            break;
        case '2':
            tim3_ch1_pwm_set_duty_cycle(20);
            current_duty_cycle = 20;  // Actualizar variable global
            uart_send_string("PWM: 20%\r\n");
            break;
        case '3':
            tim3_ch1_pwm_set_duty_cycle(30);
            current_duty_cycle = 30;  // Actualizar variable global
            uart_send_string("PWM: 30%\r\n");
            break;
        case '4':
            tim3_ch1_pwm_set_duty_cycle(40);
            current_duty_cycle = 40;  // Actualizar variable global
            uart_send_string("PWM: 40%\r\n");
            break;
        case '5':
            tim3_ch1_pwm_set_duty_cycle(50);
            current_duty_cycle = 50;  // Actualizar variable global
            uart_send_string("PWM: 50%\r\n");
            break;
        case 's':
        case 'S':
            uart_send_string("Estado actual:\r\n");
            uart_send_string(" - Lampara: ");
            uart_send_number(current_duty_cycle);
            uart_send_string("%\r\n");
            uart_send_string(" - Puerta: ");
            if (current_state == ROOM_OCCUPIED) {
                uart_send_string("Abierta\r\n");
            } else {
                uart_send_string("Cerrada\r\n");
            }
            break;
        default:
            uart_send_string("Comando desconocido: ");
            uart_send(received_char);
            uart_send_string("\r\n");
            break;
    }
}

void room_control_update(void)
{
    if (current_state == ROOM_OCCUPIED) {
        if (systick_get_ms() - led_on_time >= LED_TIMEOUT_MS) {
            current_state = ROOM_IDLE;
            tim3_ch1_pwm_set_duty_cycle(0);
            current_duty_cycle = 0;  // Actualizar variable global
            uart_send_string("Timeout: Sala vacía\r\n");
        }
    }
}

// Función auxiliar para enviar números como strings (solo valores posibles: 0,10,20,30,40,50,100)
static void uart_send_number(uint8_t number)
{
    switch (number) {
        case 0:
            uart_send_string("0");
            break;
        case 10:
            uart_send_string("10");
            break;
        case 20:
            uart_send_string("20");
            break;
        case 30:
            uart_send_string("30");
            break;
        case 40:
            uart_send_string("40");
            break;
        case 50:
            uart_send_string("50");
            break;
        case 100:
            uart_send_string("100");
            break;
        default:
            uart_send_string("??");  // Valor desconocido
            break;
    }
}
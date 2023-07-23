#pragma once

#define UART_ID uart0
#define BAUD_RATE 115200

#define UART_TX_PIN 0
#define UART_RX_PIN 1


void safe_print(const char* format, ...);

void stateless_led_blink();

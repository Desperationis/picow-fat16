#pragma once

#define UART_ID uart0
#define BAUD_RATE 115200

#define UART_TX_PIN 0
#define UART_RX_PIN 1

#define ToLittleEndian16(val) \
	 ( (((val) >> 8) & 0x00FF) | (((val) << 8) & 0xFF00) )

#define ToLittleEndian32(val) \
	 ( (((val) >> 24) & 0x000000FF) | (((val) >>  8) & 0x0000FF00) | \
	   (((val) <<  8) & 0x00FF0000) | (((val) << 24) & 0xFF000000) )



void safe_print(const char* format, ...);

void stateless_led_blink();

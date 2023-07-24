#include "util.h"
#include "device/usbd.h"
#include <stdarg.h>
#include "string.h"
#include "pico/cyw43_arch.h"
#include "hardware/uart.h"
#include "bsp/board.h"


/**
 * Slowly print out string to UART to reduce impedeance on thin wires.
 */
void safe_print(const char* format, ...) {
    va_list args;
    va_start(args, format);

    // Calculate the size of the buffer needed
    int size = vsnprintf(nullptr, 0, format, args);

    // Allocate memory for the buffer
    char* buffer = new char[size + 1];

    // Fill the buffer with the formatted string
    vsnprintf(buffer, size + 1, format, args);

    va_end(args);

	for (int i = 0;  i < strlen(buffer); i++) {
		uart_putc(UART_ID, buffer[i]);
		sleep_ms(10);
	}

    delete[] buffer;
}


void stateless_led_blink() {
	static uint32_t previous_ms = 0;
	static bool led_toggle = false;

	if ( board_millis() - previous_ms < 1000 )
		return;

	led_toggle = !led_toggle;
	previous_ms = board_millis();

	cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1 ? led_toggle : 0);
}


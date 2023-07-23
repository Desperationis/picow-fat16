#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <hardware/flash.h>
#include "device/usbd.h"
#include "tusb.h"
#include "pico.h"
#include "pico/stdlib.h"
#include "bsp/board.h"
#include "pico/cyw43_arch.h"
#include "hardware/uart.h"
#include "util.h"

int main() {
    // Initialise UART
	uart_init(UART_ID, BAUD_RATE);
	gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

    // Initialize USB Pins and Protocol. This overrides USB communication, so
    // UART must be used.
    board_init();
    tusb_init();

    if (cyw43_arch_init()) {
        safe_print("Wi-Fi init failed");
        return -1;
    }

	safe_print("--------SYSTEM START--------\n");
	safe_print("Total flash size is: %d\n", PICO_FLASH_SIZE_BYTES);
	safe_print("Sector Size %d\n", FLASH_SECTOR_SIZE);
	safe_print("Block Size %d\n", FLASH_BLOCK_SIZE);
	safe_print("Page Size %d\n", FLASH_PAGE_SIZE);


	
    /*while (true) {
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
        sleep_ms(250);
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
        sleep_ms(250);
    }*/

	while (1) {
		tud_task();
		stateless_led_blink();
	}
}



//--------------------------------------------------------------------+
// Device callbacks
//--------------------------------------------------------------------+

// Invoked when device is mounted
void tud_mount_cb(void)
{
}

// Invoked when device is unmounted
void tud_umount_cb(void)
{
}

// Invoked when usb bus is suspended
// remote_wakeup_en : if host allow us  to perform remote wakeup
// Within 7ms, device must draw an average of current less than 2.5 mA from bus
void tud_suspend_cb(bool remote_wakeup_en)
{
  (void) remote_wakeup_en;
}

// Invoked when usb bus is resumed
void tud_resume_cb(void)
{
}


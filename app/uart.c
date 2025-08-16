#include "uart.h"

// Default serial configuration
static SerialConfig uart_cfg;

// Initialize UART
void uart_init(uint32_t baudrate) {
    uart_cfg.speed = baudrate;
    uart_cfg.cr1 = 0;
    uart_cfg.cr2 = 0;
    uart_cfg.cr3 = 0;

    sdStart(&UART_DRIVER, &uart_cfg);
}

// Send a single character
void uart_put_char(char c) {
    sdPut(&UART_DRIVER, (uint8_t)c);
}

// Send a string
void uart_put_string(const char *str) {
    while (*str) {
        sdPut(&UART_DRIVER, (uint8_t)*str++);
    }
}

// Read a character (blocking)
char uart_get_char(void) {
    return (char)sdGet(&UART_DRIVER);
}

// Check if data is available
bool uart_data_available(void) {
    return (chOQGetFullI(&UART_DRIVER.iqueue) > 0);
}

#ifndef UART_H
#define UART_H

#include "ch.h"
#include "hal.h"

// UART port selection
#define UART_DRIVER SD1   // Can be SD1, SD2, SD3 depending on which USART you use

// UART configuration parameters
typedef struct {
    uint32_t baudrate;
    uint16_t cr1; // Optional for advanced config, not usually needed
} uart_config_t;

// Initialize UART with given baud rate
void uart_init(uint32_t baudrate);

// Send a single character
void uart_put_char(char c);

// Send a string (null-terminated)
void uart_put_string(const char *str);

// Read a character (blocking)
char uart_get_char(void);

// Check if data is available
bool uart_data_available(void);

#endif // UART_H

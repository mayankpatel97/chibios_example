#include "ch.h"
#include "hal.h"
#include <string.h>

/*
 * Serial configuration for USART2
 */
static const SerialConfig serial_cfg = {
  115200,
  0,
  0,
  0
};

/*
 * Helper functions for UART without chprintf
 */
void uart_send_string(const char* str) {
  while (*str) {
    sdPut(&SD2, *str++);
  }
}

void uart_send_number(uint32_t num) {
  char buffer[12];
  char *p = buffer + 11;
  *p = '\0';
  
  if (num == 0) {
    *(--p) = '0';
  } else {
    while (num > 0) {
      *(--p) = '0' + (num % 10);
      num /= 10;
    }
  }
  uart_send_string(p);
}

/*
 * UART receive buffer
 */
#define RX_BUFFER_SIZE 128
static char rx_buffer[RX_BUFFER_SIZE];

/*
 * LED blinker thread for PD12, PD13, PD14, PD15
 */
static THD_WORKING_AREA(waLedThread, 256);
static THD_FUNCTION(LedThread, arg) {
  (void)arg;
  chRegSetThreadName("led_blinker");
  
  systime_t time = chVTGetSystemTimeX();
  uint8_t pattern = 0;
  
  while (true) {
    // Clear all LEDs first
    palClearPad(GPIOD, 12);
    palClearPad(GPIOD, 13);
    palClearPad(GPIOD, 14);
    palClearPad(GPIOD, 15);
    
    // Create rotating pattern
    switch(pattern) {
      case 0:
        palSetPad(GPIOD, 12);  // PD12 ON
        break;
      case 1:
        palSetPad(GPIOD, 13);  // PD13 ON
        break;
      case 2:
        palSetPad(GPIOD, 14);  // PD14 ON
        break;
      case 3:
        palSetPad(GPIOD, 15);  // PD15 ON
        break;
      case 4:
        // All LEDs ON
        palSetPad(GPIOD, 12);
        palSetPad(GPIOD, 13);
        palSetPad(GPIOD, 14);
        palSetPad(GPIOD, 15);
        break;
    }
    
    pattern = (pattern + 1) % 5;  // Cycle through 0-4
    
    // Wait 300ms
    time = chThdSleepUntilWindowed(time, time + TIME_MS2I(300));
  }
}

/*
 * UART receiver thread
 */
static THD_WORKING_AREA(waUartRxThread, 256);
static THD_FUNCTION(UartRxThread, arg) {
  (void)arg;
  chRegSetThreadName("uart_rx");
  
  char c;
  int rx_index = 0;
  
  while (true) {
    // Read one character (blocking)
    c = sdGet(&SD2);
    
    if (c == '\r' || c == '\n') {
      // End of line received
      if (rx_index > 0) {
        rx_buffer[rx_index] = '\0';
        
        // Echo received string
        uart_send_string("Received: ");
        uart_send_string(rx_buffer);
        uart_send_string("\r\n");
        
        // Process commands
        if (strcmp(rx_buffer, "led") == 0) {
          palTogglePad(GPIOD, GPIOD_LED4);
          uart_send_string("LED4 toggled\r\n");
        }
        else if (strcmp(rx_buffer, "led12") == 0) {
          palTogglePad(GPIOD, 12);
          uart_send_string("PD12 toggled\r\n");
        }
        else if (strcmp(rx_buffer, "led13") == 0) {
          palTogglePad(GPIOD, 13);
          uart_send_string("PD13 toggled\r\n");
        }
        else if (strcmp(rx_buffer, "led14") == 0) {
          palTogglePad(GPIOD, 14);
          uart_send_string("PD14 toggled\r\n");
        }
        else if (strcmp(rx_buffer, "led15") == 0) {
          palTogglePad(GPIOD, 15);
          uart_send_string("PD15 toggled\r\n");
        }
        else if (strcmp(rx_buffer, "all") == 0) {
          palTogglePad(GPIOD, 12);
          palTogglePad(GPIOD, 13);
          palTogglePad(GPIOD, 14);
          palTogglePad(GPIOD, 15);
          uart_send_string("All LEDs toggled\r\n");
        }
        else if (strcmp(rx_buffer, "help") == 0) {
          uart_send_string("Commands: led, led12, led13, led14, led15, all, help\r\n");
        }
        
        rx_index = 0;
      }
    } else if (rx_index < (RX_BUFFER_SIZE - 1)) {
      // Store character
      rx_buffer[rx_index++] = c;
      
      // Echo character
      sdPut(&SD2, c);
    }
  }
}

/*
 * UART transmit thread (periodic messages)
 */
static THD_WORKING_AREA(waUartTxThread, 256);
static THD_FUNCTION(UartTxThread, arg) {
  (void)arg;
  chRegSetThreadName("uart_tx");
  
  systime_t time = chVTGetSystemTimeX();
  uint32_t counter = 0;
  
  while (true) {
    // Send periodic message
    uart_send_string("Counter: ");
    uart_send_number(counter++);
    uart_send_string("\r\n");
    
    // Wait 3 seconds
    time = chThdSleepUntilWindowed(time, time + TIME_MS2I(3000));
  }
}

/*
 * Application entry point
 */
int main(void) {
  /*
   * System initializations
   */
  halInit();
  chSysInit();

  /*
   * Configure UART pins
   * PA2 - TX (AF7)
   * PA3 - RX (AF7)
   */
  palSetPadMode(GPIOA, 2, PAL_MODE_ALTERNATE(7));  // TX
  palSetPadMode(GPIOA, 3, PAL_MODE_ALTERNATE(7));  // RX

  /*
   * Start serial driver
   */
  sdStart(&SD2, &serial_cfg);

  /*
   * Configure LED pins (PD12, PD13, PD14, PD15)
   */
  palSetPadMode(GPIOD, 12, PAL_MODE_OUTPUT_PUSHPULL);  // PD12
  palSetPadMode(GPIOD, 13, PAL_MODE_OUTPUT_PUSHPULL);  // PD13
  palSetPadMode(GPIOD, 14, PAL_MODE_OUTPUT_PUSHPULL);  // PD14
  palSetPadMode(GPIOD, 15, PAL_MODE_OUTPUT_PUSHPULL);  // PD15
  palSetPadMode(GPIOD, GPIOD_LED4, PAL_MODE_OUTPUT_PUSHPULL);  // Board LED

  /*
   * Initialize all LEDs to OFF
   */
  palClearPad(GPIOD, 12);
  palClearPad(GPIOD, 13);
  palClearPad(GPIOD, 14);
  palClearPad(GPIOD, 15);

  /*
   * Send startup message
   */
  chThdSleepMilliseconds(100);  // Let UART stabilize
  uart_send_string("\r\n*** ChibiOS UART Example with Multiple LEDs ***\r\n");
  uart_send_string("Commands:\r\n");
  uart_send_string("  'led'   - Toggle board LED\r\n");
  uart_send_string("  'led12' - Toggle PD12\r\n");
  uart_send_string("  'led13' - Toggle PD13\r\n");
  uart_send_string("  'led14' - Toggle PD14\r\n");
  uart_send_string("  'led15' - Toggle PD15\r\n");
  uart_send_string("  'all'   - Toggle all LEDs\r\n");
  uart_send_string("  'help'  - Show this help\r\n");

  /*
   * Create threads
   */
  chThdCreateStatic(waUartRxThread, sizeof(waUartRxThread), 
                    NORMALPRIO + 2, UartRxThread, NULL);
  chThdCreateStatic(waUartTxThread, sizeof(waUartTxThread), 
                    NORMALPRIO, UartTxThread, NULL);
  chThdCreateStatic(waLedThread, sizeof(waLedThread), 
                    NORMALPRIO + 1, LedThread, NULL);

  /*
   * Main thread - heartbeat with board LED
   */
  while (true) {
    palTogglePad(GPIOD, GPIOD_LED3);  // Heartbeat LED
    chThdSleepMilliseconds(2000);     // Slower heartbeat
  }
}
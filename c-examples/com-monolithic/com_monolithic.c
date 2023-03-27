// com_monolithic.c
// TAB C Example COM Monolithic
//
// Written by Bradley Denby
// Other contributors: Abhishek Anand, Alexey Tatarinov
//
// See the top-level LICENSE file for the license.

// Standard library headers
#include <stddef.h> // size_t
#include <stdint.h> // uint8_t, uint32_t

// Board-specific header
#include <com.h>    // COM header

// TAB header
#include <tab.h>    // TAB header

// Main
int main(void) {
  // MCU initialization
  init_clock();
  init_leds();
  init_uart();
  init_radio();
  // TAB initialization
  rx_cmd_buff_t rx_cmd_buff = {.size=CMD_MAX_LEN};
  clear_rx_cmd_buff(&rx_cmd_buff);
  tx_cmd_buff_t tx_cmd_buff = {.size=CMD_MAX_LEN};
  clear_tx_cmd_buff(&tx_cmd_buff);

  rx_cmd_buff_t radio_rx_cmd_buff = {.size=CMD_MAX_LEN};
  clear_rx_cmd_buff(&radio_rx_cmd_buff);
  tx_cmd_buff_t radio_tx_cmd_buff = {.size=CMD_MAX_LEN};
  clear_tx_cmd_buff(&radio_tx_cmd_buff);

  // TAB loop
  while(1) {
    rx_uart0(&rx_cmd_buff);            // Collect command bytes
    forward(&rx_cmd_buff, &radio_tx_cmd_buff, 0);
    radio_transceive(&radio_rx_cmd_buff, &radio_tx_cmd_buff);
    forward(&radio_rx_cmd_buff, &tx_cmd_buff, 1);
    tx_uart0(&tx_cmd_buff);
  }
  // Should never reach this point
  return 0;
}

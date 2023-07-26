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
  rx_cmd_buff_t uart_rx_cmd_buff = {.size=CMD_MAX_LEN, .handles_offboard=false, .reply_nibble=CDH};
  clear_rx_cmd_buff(&uart_rx_cmd_buff);
  tx_cmd_buff_t uart_tx_cmd_buff = {.size=CMD_MAX_LEN};
  clear_tx_cmd_buff(&uart_tx_cmd_buff);

  rx_cmd_buff_t radio_rx_cmd_buff = {.size=CMD_MAX_LEN, .handles_offboard=true};
  clear_rx_cmd_buff(&radio_rx_cmd_buff);
  tx_cmd_buff_t radio_tx_cmd_buff = {.size=CMD_MAX_LEN};
  clear_tx_cmd_buff(&radio_tx_cmd_buff);

  liveness_accountant_t liveness_accountant = {.size=MAX_LIVE};
  for (size_t i = 0; i < MAX_LIVE; i++) {
    liveness_accountant.data[i] = 0;
  }

  #define LEN_TX_BUFFS 2
  tx_cmd_buff_t* tx_cmd_buffs_o[LEN_TX_BUFFS];
  tx_cmd_buffs_o[INDEX_UART] = &uart_tx_cmd_buff;
  tx_cmd_buffs_o[INDEX_RADIO] = &radio_tx_cmd_buff;

  // TAB loop
  while(1) {
    rx_uart0(&uart_rx_cmd_buff);            // Collect command bytes
    tx_uart0(&uart_tx_cmd_buff);
    radio_transceive(&radio_rx_cmd_buff, &radio_tx_cmd_buff);

    route(&uart_rx_cmd_buff, &liveness_accountant, LEN_TX_BUFFS, tx_cmd_buffs_o);
    route(&radio_rx_cmd_buff, &liveness_accountant, LEN_TX_BUFFS, tx_cmd_buffs_o);
  }
  // Should never reach this point
  return 0;
}

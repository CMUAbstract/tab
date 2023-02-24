// com.h
// COM board support header file
//
// Written by Bradley Denby
// Other contributors: Abhishek Anand, Chad Taylor, Alexey Tatarinov
//
// See the top-level LICENSE file for the license.

#ifndef COM_H
#define COM_H

// TAB header
#include <tab.h> // common_data_t, rx_cmd_buff_t, tx_cmd_buff_t

// UART pin locations
#define TXD GPIO21
#define RXD GPIO22
#define CTS GPIO23
#define RTS GPIO24

// Functions required by TAB

int handle_common_data(common_data_t common_data_buff_i);
int handle_bootloader_erase(void);
int bootloader_active(void);

// Board initialization functions

void init_clock(void);
void init_led(void);
void init_uart(void);

// Feature functions

void rx_uart0(rx_cmd_buff_t* rx_cmd_buff_o);
void reply(rx_cmd_buff_t* rx_cmd_buff_o, tx_cmd_buff_t* tx_cmd_buff_o);
void tx_uart0(tx_cmd_buff_t* tx_cmd_buff_o);

#endif

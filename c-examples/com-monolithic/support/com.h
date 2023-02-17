// com.h
// COM board support header file
//
// Written by Bradley Denby
// Other contributors: Chad Taylor
//
// See the top-level LICENSE file for the license.

#ifndef COM_H
#define COM_H

// TAB header
#include <tab.h> // rx_cmd_buff_t, tx_cmd_buff_t, common_data_t

//// Byte counts
#define BYTES_PER_WORD ((uint32_t)4)
#define BYTES_PER_CMD  ((uint32_t)128)
#define BYTES_PER_PAGE ((uint32_t)2048)

// Functions required by TAB
int handle_common_data(common_data_t common_data_buff_i);
int bootloader_running(void);
int handle_bootloader_erase(void);

// Board initialization functions

void init_clock(void);
void init_uart(void);

// Feature functions

void rx_usart1(rx_cmd_buff_t* rx_cmd_buff_o);
void reply(rx_cmd_buff_t* rx_cmd_buff_o, tx_cmd_buff_t* tx_cmd_buff_o);
void tx_usart1(tx_cmd_buff_t* tx_cmd_buff_o);

#endif

// cdh.h
// CDH board support header file
//
// Written by Bradley Denby
// Other contributors: None
//
// See the top-level LICENSE file for the license.

#ifndef CDH_H
#define CDH_H

// TAB header
#include <tab.h> // rx_cmd_buff_t, tx_cmd_buff_t, common_data_t

// Initialization functions

void init_clock(void);
void init_uart(void);

// Task-like functions

void rx_usart1(rx_cmd_buff_t* rx_cmd_buff_o);
void reply(rx_cmd_buff_t* rx_cmd_buff_o, tx_cmd_buff_t* tx_cmd_buff_o);
void tx_usart1(tx_cmd_buff_t* tx_cmd_buff_o);

#endif

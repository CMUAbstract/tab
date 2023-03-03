// cdh.h
// CDH board support header file
//
// Written by Bradley Denby
// Other contributors: Chad Taylor, Alok Anand
//
// See the top-level LICENSE file for the license.

#ifndef CDH_H
#define CDH_H

// TAB header
#include <tab.h> // common_data_t, rx_cmd_buff_t, tx_cmd_buff_t

// Macros

//// Byte counts
#define BYTES_PER_BLR_PLD    ((uint32_t)128)
#define BYTES_PER_FLASH_PAGE ((uint32_t)2048)

//// Start of application address space
#define APP_ADDR   ((uint32_t)0x08008000U)

//// SRAM1 start address
#define SRAM1_BASE ((uint32_t)0x20000000U)

//// SRAM1 size
#define SRAM1_SIZE ((uint32_t)0x00040000U)

// Functions required by TAB

int handle_common_data(common_data_t common_data_buff_i);
int handle_bootloader_erase(void);
int handle_bootloader_write_page(rx_cmd_buff_t* rx_cmd_buff);
int handle_bootloader_write_page_addr32(rx_cmd_buff_t* rx_cmd_buff);
int handle_bootloader_jump(void);
int bootloader_active(void);

// Board initialization functions

void init_clock(void);
void init_leds(void);
void init_uart(void);

// Feature functions

void rx_usart1(rx_cmd_buff_t* rx_cmd_buff_o);
void reply(rx_cmd_buff_t* rx_cmd_buff_o, tx_cmd_buff_t* tx_cmd_buff_o);
void tx_usart1(tx_cmd_buff_t* tx_cmd_buff_o);

#endif

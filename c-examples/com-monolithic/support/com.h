// com.h
// COM board support header file
//
// Written by Bradley Denby
// Other contributors: Abhishek Anand, Chad Taylor, Alexey Tatarinov
//
// See the top-level LICENSE file for the license.

#ifndef COM_H
#define COM_H

// libopencm3 library
#include <libopencm3/nrf/common/memorymap.h> // NVMC_BASE

// TAB header
#include <tab.h> // common_data_t, rx_cmd_buff_t, tx_cmd_buff_t

// Macros

//// Byte counts
#define BYTES_PER_BLR_PLD    ((uint32_t)128)
#define BYTES_PER_FLASH_PAGE ((uint32_t)1024)

//// Start of application address space
#define APP_ADDR ((uint32_t)0x00008000U)

//// Missing from libopencm3
#define NVMC_READY     MMIO32(NVMC_BASE+0x400)
#define NVMC_CONFIG    MMIO32(NVMC_BASE+0x504)
#define NVMC_ERASEPCR1 MMIO32(NVMC_BASE+0x508)

//// Aliases for NVMC CONFIG
#define REN 0
#define WEN 1
#define EEN 2

//// Aliases for GPIO UART pins
#define TXD GPIO21
#define RXD GPIO22
#define CTS GPIO23
#define RTS GPIO24

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

void rx_uart0(rx_cmd_buff_t* rx_cmd_buff_o);
void reply(rx_cmd_buff_t* rx_cmd_buff_o, tx_cmd_buff_t* tx_cmd_buff_o);
void tx_uart0(tx_cmd_buff_t* tx_cmd_buff_o);

#endif

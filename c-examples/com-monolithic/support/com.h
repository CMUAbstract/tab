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

// Macros
// Needed for flash and erase
#define NVMC_READY_READY_Busy (0UL) /*!< NVMC is busy (on-going write or erase operation). */
#define NVMC_READY_READY_Ready (1UL) /*!< NVMC is ready. */
/* Bits 1..0 : Program write enable. */
#define NVMC_CONFIG_WEN_Pos (0UL) /*!< Position of WEN field. */
#define NVMC_CONFIG_WEN_Msk (0x3UL << NVMC_CONFIG_WEN_Pos) /*!< Bit mask of WEN field. */
#define NVMC_CONFIG_WEN_Ren (0x00UL) /*!< Read only access. */
#define NVMC_CONFIG_WEN_Wen (0x01UL) /*!< Write enabled. */
#define NVMC_CONFIG_WEN_Een (0x02UL) /*!< Erase enabled. */

typedef struct {                               /*!< NVMC Structure                                                        */
  volatile const uint32_t  RESERVED0[256];
  volatile const uint32_t  READY;              /*!< Ready flag.                                                           */
  volatile const uint32_t  RESERVED1[64];
  volatile uint32_t        CONFIG;             /*!< Configuration register.                                               */
  
  union {
    volatile uint32_t      ERASEPCR1;          /*!< Register for erasing a non-protected non-volatile memory page.        */
    volatile uint32_t      ERASEPAGE;          /*!< Register for erasing a non-protected non-volatile memory page.        */
  };
  volatile uint32_t        ERASEALL;           /*!< Register for erasing all non-volatile user memory.                    */
  volatile uint32_t        ERASEPCR0;          /*!< Register for erasing a protected non-volatile memory page.            */
  volatile uint32_t        ERASEUICR;          /*!< Register for start erasing User Information Congfiguration Registers. */
} NRF_NVMC_Type;

#define NRF_NVMC   ((NRF_NVMC_Type           *) NVMC_BASE)

//// Start of application address space
#define APP_ADDR   ((uint32_t)0x00008000U)

//// Byte counts
#define BYTES_PER_BLR_PLD    ((uint32_t)128)

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
void init_led(void);
void init_uart(void);

// Feature functions

void rx_uart0(rx_cmd_buff_t* rx_cmd_buff_o);
void reply(rx_cmd_buff_t* rx_cmd_buff_o, tx_cmd_buff_t* tx_cmd_buff_o);
void tx_uart0(tx_cmd_buff_t* tx_cmd_buff_o);

#endif

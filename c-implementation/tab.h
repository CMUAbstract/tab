// tab.h
// TAB serial communication protocol header file
//
// Written by Bradley Denby
// Other contributors: Chad Taylor
//
// See the top-level LICENSE file for the license.

#ifndef TAB_H
#define TAB_H

// Standard library
#include <stddef.h> // size_t
#include <stdint.h> // uint8_t, uint32_t

// Macros

//// General
#define CMD_MAX_LEN  ((size_t)258)
#define PLD_MAX_LEN  ((size_t)249)
#define START_BYTE_0 ((uint8_t)0x22)
#define START_BYTE_1 ((uint8_t)0x69)

//// Opcodes
#define COMMON_ACK_OPCODE       ((uint8_t)0x10)
#define COMMON_NACK_OPCODE      ((uint8_t)0xff)
#define COMMON_DEBUG_OPCODE     ((uint8_t)0x11)
#define COMMON_DATA_OPCODE      ((uint8_t)0x16)
#define BOOTLOADER_ACK_OPCODE   ((uint8_t)0x01)
#define BOOTLOADER_NACK_OPCODE  ((uint8_t)0x0f)
#define BOOTLOADER_PING_OPCODE  ((uint8_t)0x00)

//// BOOTLOADER_ACK reasons
#define BOOTLOADER_ACK_REASON_PONG   ((uint8_t)0x00)
#define BOOTLOADER_ACK_REASON_ERASED ((uint8_t)0x01)
#define BOOTLOADER_ACK_REASON_JUMP   ((uint8_t)0xff)

//// Route Nibble IDs
#define GND ((uint8_t)0x00)
#define COM ((uint8_t)0x01)
#define CDH ((uint8_t)0x02)
#define PLD ((uint8_t)0x03)

// Typedefs

//// TAB command indices
typedef enum cmd_index {
  START_BYTE_0_INDEX = ((size_t)0),
  START_BYTE_1_INDEX = ((size_t)1),
  MSG_LEN_INDEX      = ((size_t)2),
  HWID_LSB_INDEX     = ((size_t)3),
  HWID_MSB_INDEX     = ((size_t)4),
  MSG_ID_LSB_INDEX   = ((size_t)5),
  MSG_ID_MSB_INDEX   = ((size_t)6),
  ROUTE_INDEX        = ((size_t)7),
  OPCODE_INDEX       = ((size_t)8),
  PLD_START_INDEX    = ((size_t)9)
} cmd_index_t;

//// RX command buffer states
typedef enum rx_cmd_buff_state {
  RX_CMD_BUFF_STATE_START_BYTE_0 = ((uint8_t)0x00),
  RX_CMD_BUFF_STATE_START_BYTE_1 = ((uint8_t)0x01),
  RX_CMD_BUFF_STATE_MSG_LEN      = ((uint8_t)0x02),
  RX_CMD_BUFF_STATE_HWID_LSB     = ((uint8_t)0x03),
  RX_CMD_BUFF_STATE_HWID_MSB     = ((uint8_t)0x04),
  RX_CMD_BUFF_STATE_MSG_ID_LSB   = ((uint8_t)0x05),
  RX_CMD_BUFF_STATE_MSG_ID_MSB   = ((uint8_t)0x06),
  RX_CMD_BUFF_STATE_ROUTE        = ((uint8_t)0x07),
  RX_CMD_BUFF_STATE_OPCODE       = ((uint8_t)0x08),
  RX_CMD_BUFF_STATE_PLD          = ((uint8_t)0x09),
  RX_CMD_BUFF_STATE_COMPLETE     = ((uint8_t)0x0a)
} rx_cmd_buff_state_t;

//// Common Data buffer
typedef struct common_data_buff {
  size_t       end_index;         // data[i] valid for i<end_index
  const size_t size;              // common_data_t b={.size=PLD_MAX_LEN};
  uint8_t      data[PLD_MAX_LEN]; // Payload bytes
} common_data_t;

//// RX command buffer
typedef struct rx_cmd_buff {
  rx_cmd_buff_state_t state;             // See enum rx_cmd_buff_state
  size_t              start_index;       // Index of next byte to be buffered
  size_t              end_index;         // data[i] valid for i<end_index
  const size_t        size;              // rx_cmd_buff_t b={.size=CMD_MAX_LEN};
  uint8_t             data[CMD_MAX_LEN]; // Command bytes
} rx_cmd_buff_t;

//// TX command buffer
typedef struct tx_cmd_buff {
  int          empty;             // Whether or not tx_cmd_buff contains a cmd
  size_t       start_index;       // Index of next byte to be sent
  size_t       end_index;         // data[i] valid for i<end_index
  const size_t size;              // tx_cmd_buff_t b={.size=CMD_MAX_LEN};
  uint8_t      data[CMD_MAX_LEN]; // Command bytes
} tx_cmd_buff_t;

// Helper functions

//// Clears rx_cmd_buff data and resets state and indices
void clear_rx_cmd_buff(rx_cmd_buff_t* rx_cmd_buff_o);

//// Clears tx_cmd_buff data and resets state and indices
void clear_tx_cmd_buff(tx_cmd_buff_t* tx_cmd_buff_o);

// Protocol functions

//// Attempts to push byte to end of rx_cmd_buff
void push_rx_cmd_buff(rx_cmd_buff_t* rx_cmd_buff_o, uint8_t b);

//// Attempts to clear rx_cmd_buff and populate tx_cmd_buff with reply
void write_reply(rx_cmd_buff_t* rx_cmd_buff_o, tx_cmd_buff_t* tx_cmd_buff_o);

//// Attempts to pop byte from beginning of tx_cmd_buff
uint8_t pop_tx_cmd_buff(tx_cmd_buff_t* tx_cmd_buff_o);

#endif

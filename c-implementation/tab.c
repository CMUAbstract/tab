// tab.c
// TAB serial communication protocol header file
//
// Written by Bradley Denby
// Other contributors: Chad Taylor
//
// See the top-level LICENSE file for the license.

// Standard library
#include <stddef.h> // size_t
#include <stdint.h> // uint8_t, uint32_t

// TAB
#include <tab.h>    // TAB header

// External handler functions

extern int handle_common_data(common_data_t common_data_buff_i);
extern int handle_bootloader_erase(void);
extern int handle_bootloader_write_page(rx_cmd_buff_t* rx_cmd_buff);
extern int handle_bootloader_write_page_addr32(rx_cmd_buff_t* rx_cmd_buff);
extern int handle_bootloader_jump(void);
extern int handle_app_set_time(const uint32_t sec, const uint32_t ns);
extern int handle_app_get_time(uint32_t* sec, uint32_t* ns);
extern int bootloader_active(void);

// Helper functions

//// Clears rx_cmd_buff data and resets state and indices
void clear_rx_cmd_buff(rx_cmd_buff_t* rx_cmd_buff_o) {
  rx_cmd_buff_o->state = RX_CMD_BUFF_STATE_START_BYTE_0;
  rx_cmd_buff_o->start_index = 0;
  rx_cmd_buff_o->end_index = 0;
  for(size_t i=0; i<rx_cmd_buff_o->size; i++) {
    rx_cmd_buff_o->data[i] = ((uint8_t)0x00);
  }
}

//// Clears tx_cmd_buff data and resets state and indices
void clear_tx_cmd_buff(tx_cmd_buff_t* tx_cmd_buff_o) {
  tx_cmd_buff_o->empty = 1;
  tx_cmd_buff_o->start_index = 0;
  tx_cmd_buff_o->end_index = 0;
  for(size_t i=0; i<tx_cmd_buff_o->size; i++) {
    tx_cmd_buff_o->data[i] = ((uint8_t)0x00);
  }
}

// Protocol functions

//// Attempts to push byte to end of rx_cmd_buff
void push_rx_cmd_buff(rx_cmd_buff_t* rx_cmd_buff_o, uint8_t b) {
  switch(rx_cmd_buff_o->state) {
    case RX_CMD_BUFF_STATE_START_BYTE_0:
      if(b==START_BYTE_0) {
        rx_cmd_buff_o->data[START_BYTE_0_INDEX] = b;
        rx_cmd_buff_o->state = RX_CMD_BUFF_STATE_START_BYTE_1;
      }
      break;
    case RX_CMD_BUFF_STATE_START_BYTE_1:
      if(b==START_BYTE_1) {
        rx_cmd_buff_o->data[START_BYTE_1_INDEX] = b;
        rx_cmd_buff_o->state = RX_CMD_BUFF_STATE_MSG_LEN;
      } else {
        clear_rx_cmd_buff(rx_cmd_buff_o);
      }
      break;
    case RX_CMD_BUFF_STATE_MSG_LEN:
      if(((uint8_t)0x06)<=b) {
        rx_cmd_buff_o->data[MSG_LEN_INDEX] = b;
        rx_cmd_buff_o->start_index = ((uint8_t)0x09);
        rx_cmd_buff_o->end_index = (b+((uint8_t)0x03));
        rx_cmd_buff_o->state = RX_CMD_BUFF_STATE_HWID_LSB;
      } else {
        clear_rx_cmd_buff(rx_cmd_buff_o);
      }
      break;
    case RX_CMD_BUFF_STATE_HWID_LSB:
      rx_cmd_buff_o->data[HWID_LSB_INDEX] = b;
      rx_cmd_buff_o->state = RX_CMD_BUFF_STATE_HWID_MSB;
      break;
    case RX_CMD_BUFF_STATE_HWID_MSB:
      rx_cmd_buff_o->data[HWID_MSB_INDEX] = b;
      rx_cmd_buff_o->state = RX_CMD_BUFF_STATE_MSG_ID_LSB;
      break;
    case RX_CMD_BUFF_STATE_MSG_ID_LSB:
      rx_cmd_buff_o->data[MSG_ID_LSB_INDEX] = b;
      rx_cmd_buff_o->state = RX_CMD_BUFF_STATE_MSG_ID_MSB;
      break;
    case RX_CMD_BUFF_STATE_MSG_ID_MSB:
      rx_cmd_buff_o->data[MSG_ID_MSB_INDEX] = b;
      rx_cmd_buff_o->state = RX_CMD_BUFF_STATE_ROUTE;
      break;
    case RX_CMD_BUFF_STATE_ROUTE:
      rx_cmd_buff_o->data[ROUTE_INDEX] = b;
      rx_cmd_buff_o->state = RX_CMD_BUFF_STATE_OPCODE;
      break;
    case RX_CMD_BUFF_STATE_OPCODE: // no check for valid opcodes (too much)
      rx_cmd_buff_o->data[OPCODE_INDEX] = b;
      if(rx_cmd_buff_o->start_index<rx_cmd_buff_o->end_index) {
        rx_cmd_buff_o->state = RX_CMD_BUFF_STATE_PLD;
      } else {
        rx_cmd_buff_o->state = RX_CMD_BUFF_STATE_COMPLETE;
      }
      break;
    case RX_CMD_BUFF_STATE_PLD:
      if(rx_cmd_buff_o->start_index<rx_cmd_buff_o->end_index) {
        rx_cmd_buff_o->data[rx_cmd_buff_o->start_index] = b;
        rx_cmd_buff_o->start_index += 1;
      }
      // Must move to COMPLETE state immediately if b is the last byte
      if(rx_cmd_buff_o->start_index==rx_cmd_buff_o->end_index) {
        rx_cmd_buff_o->state = RX_CMD_BUFF_STATE_COMPLETE;
      }
      break;
    case RX_CMD_BUFF_STATE_COMPLETE:
      // If rx_cmd_buff_t holds a complete command, do nothing with new byte b
      break;
    default:
      // Do nothing by default
      break;
  }
}

//// Attempts to clear rx_cmd_buff and populate tx_cmd_buff with reply
void write_reply(rx_cmd_buff_t* rx_cmd_buff_o, tx_cmd_buff_t* tx_cmd_buff_o) {
  if(
   rx_cmd_buff_o->state==RX_CMD_BUFF_STATE_COMPLETE &&
   tx_cmd_buff_o->empty
  ) {
    tx_cmd_buff_o->data[START_BYTE_0_INDEX] = START_BYTE_0;
    tx_cmd_buff_o->data[START_BYTE_1_INDEX] = START_BYTE_1;
    tx_cmd_buff_o->data[HWID_LSB_INDEX] = rx_cmd_buff_o->data[HWID_LSB_INDEX];
    tx_cmd_buff_o->data[HWID_MSB_INDEX] = rx_cmd_buff_o->data[HWID_MSB_INDEX];
    tx_cmd_buff_o->data[MSG_ID_LSB_INDEX] =
     rx_cmd_buff_o->data[MSG_ID_LSB_INDEX];
    tx_cmd_buff_o->data[MSG_ID_MSB_INDEX] =
     rx_cmd_buff_o->data[MSG_ID_MSB_INDEX];
    tx_cmd_buff_o->data[ROUTE_INDEX] =
     (0x0f & rx_cmd_buff_o->data[ROUTE_INDEX]) << 4 |
     (0xf0 & rx_cmd_buff_o->data[ROUTE_INDEX]) >> 4;
    // useful variables
    size_t i = 0;
    uint8_t sec_0;
    uint8_t sec_1;
    uint8_t sec_2;
    uint8_t sec_3;
    uint8_t ns_0;
    uint8_t ns_1;
    uint8_t ns_2;
    uint8_t ns_3;
    uint32_t sec;
    uint32_t ns;
    common_data_t common_data_buff = {.size=PLD_MAX_LEN};
    int success = 0;
    switch(rx_cmd_buff_o->data[OPCODE_INDEX]) {
      case COMMON_ACK_OPCODE:
        tx_cmd_buff_o->data[MSG_LEN_INDEX] = ((uint8_t)0x06);
        tx_cmd_buff_o->data[OPCODE_INDEX] = COMMON_ACK_OPCODE;
        break;
      case COMMON_NACK_OPCODE:
        tx_cmd_buff_o->data[MSG_LEN_INDEX] = ((uint8_t)0x06);
        tx_cmd_buff_o->data[OPCODE_INDEX] = COMMON_NACK_OPCODE;
        break;
      case COMMON_DEBUG_OPCODE:
        tx_cmd_buff_o->data[MSG_LEN_INDEX] = rx_cmd_buff_o->data[MSG_LEN_INDEX];
        tx_cmd_buff_o->data[OPCODE_INDEX] = COMMON_DEBUG_OPCODE;
        for(i=PLD_START_INDEX; i<rx_cmd_buff_o->end_index; i++) {
          tx_cmd_buff_o->data[i] = rx_cmd_buff_o->data[i];
        }
        break;
      case COMMON_DATA_OPCODE:
        // handle common_data
        for(i=PLD_START_INDEX; i<rx_cmd_buff_o->end_index; i++) {
          common_data_buff.data[i-PLD_START_INDEX] = rx_cmd_buff_o->data[i];
        }
        common_data_buff.end_index = rx_cmd_buff_o->end_index-PLD_START_INDEX;
        success = handle_common_data(common_data_buff);
        // reply
        if(success) {
          tx_cmd_buff_o->data[MSG_LEN_INDEX] = ((uint8_t)0x06);
          tx_cmd_buff_o->data[OPCODE_INDEX] = COMMON_ACK_OPCODE;
        } else {
          tx_cmd_buff_o->data[MSG_LEN_INDEX] = ((uint8_t)0x06);
          tx_cmd_buff_o->data[OPCODE_INDEX] = COMMON_NACK_OPCODE;
        }
        break;
      case BOOTLOADER_ACK_OPCODE:
        tx_cmd_buff_o->data[MSG_LEN_INDEX] = ((uint8_t)0x06);
        tx_cmd_buff_o->data[OPCODE_INDEX] = COMMON_NACK_OPCODE;
        break;
      case BOOTLOADER_NACK_OPCODE:
        tx_cmd_buff_o->data[MSG_LEN_INDEX] = ((uint8_t)0x06);
        tx_cmd_buff_o->data[OPCODE_INDEX] = COMMON_NACK_OPCODE;
        break;
      case BOOTLOADER_PING_OPCODE:
        if(bootloader_active()) {
          tx_cmd_buff_o->data[MSG_LEN_INDEX] = ((uint8_t)0x07);
          tx_cmd_buff_o->data[OPCODE_INDEX] = BOOTLOADER_ACK_OPCODE;
          tx_cmd_buff_o->data[PLD_START_INDEX] = BOOTLOADER_ACK_REASON_PONG;
        } else {
          tx_cmd_buff_o->data[MSG_LEN_INDEX] = ((uint8_t)0x06);
          tx_cmd_buff_o->data[OPCODE_INDEX] = COMMON_NACK_OPCODE;
        }
        break;
      case BOOTLOADER_ERASE_OPCODE:
        if(bootloader_active()) {
          success = handle_bootloader_erase();
          if(success) {
            tx_cmd_buff_o->data[MSG_LEN_INDEX] = ((uint8_t)0x07);
            tx_cmd_buff_o->data[OPCODE_INDEX] = BOOTLOADER_ACK_OPCODE;
            tx_cmd_buff_o->data[PLD_START_INDEX] = BOOTLOADER_ACK_REASON_ERASED;
          } else {
            tx_cmd_buff_o->data[MSG_LEN_INDEX] = ((uint8_t)0x06);
            tx_cmd_buff_o->data[OPCODE_INDEX] = BOOTLOADER_NACK_OPCODE;
          }
        } else {
          tx_cmd_buff_o->data[MSG_LEN_INDEX] = ((uint8_t)0x06);
          tx_cmd_buff_o->data[OPCODE_INDEX] = COMMON_NACK_OPCODE;
        }
        break;
      case BOOTLOADER_WRITE_PAGE_OPCODE:
        if(bootloader_active()) {
          success = handle_bootloader_write_page(rx_cmd_buff_o);
          if(success) {
            tx_cmd_buff_o->data[MSG_LEN_INDEX] = ((uint8_t)0x07);
            tx_cmd_buff_o->data[OPCODE_INDEX] = BOOTLOADER_ACK_OPCODE;
            tx_cmd_buff_o->data[PLD_START_INDEX] =
             rx_cmd_buff_o->data[PLD_START_INDEX];
          } else {
            tx_cmd_buff_o->data[MSG_LEN_INDEX] = ((uint8_t)0x06);
            tx_cmd_buff_o->data[OPCODE_INDEX] = BOOTLOADER_NACK_OPCODE;
          }
        } else{
          tx_cmd_buff_o->data[MSG_LEN_INDEX] = ((uint8_t)0x06);
          tx_cmd_buff_o->data[OPCODE_INDEX] = COMMON_NACK_OPCODE;
        }
        break;
      case BOOTLOADER_WRITE_PAGE_ADDR32_OPCODE:
        if(bootloader_active()) {
          success = handle_bootloader_write_page_addr32(rx_cmd_buff_o);
          if(success) {
            tx_cmd_buff_o->data[MSG_LEN_INDEX] = ((uint8_t)0x0a);
            tx_cmd_buff_o->data[OPCODE_INDEX] = BOOTLOADER_ACK_OPCODE;
            tx_cmd_buff_o->data[PLD_START_INDEX] =
             rx_cmd_buff_o->data[PLD_START_INDEX];
            tx_cmd_buff_o->data[PLD_START_INDEX+1] =
             rx_cmd_buff_o->data[PLD_START_INDEX+1];
            tx_cmd_buff_o->data[PLD_START_INDEX+2] =
             rx_cmd_buff_o->data[PLD_START_INDEX+2];
            tx_cmd_buff_o->data[PLD_START_INDEX+3] =
             rx_cmd_buff_o->data[PLD_START_INDEX+3];
          } else {
            tx_cmd_buff_o->data[MSG_LEN_INDEX] = ((uint8_t)0x06);
            tx_cmd_buff_o->data[OPCODE_INDEX] = BOOTLOADER_NACK_OPCODE;
          }
        } else{
          tx_cmd_buff_o->data[MSG_LEN_INDEX] = ((uint8_t)0x06);
          tx_cmd_buff_o->data[OPCODE_INDEX] = COMMON_NACK_OPCODE;
        }
        break;
      case BOOTLOADER_JUMP_OPCODE:
        if(bootloader_active()) {
          success = handle_bootloader_jump();
          if(success) {
            tx_cmd_buff_o->data[MSG_LEN_INDEX] = ((uint8_t)0x07);
            tx_cmd_buff_o->data[OPCODE_INDEX] = BOOTLOADER_ACK_OPCODE;
            tx_cmd_buff_o->data[PLD_START_INDEX] = BOOTLOADER_ACK_REASON_JUMPED;
          } else {
            tx_cmd_buff_o->data[MSG_LEN_INDEX] = ((uint8_t)0x06);
            tx_cmd_buff_o->data[OPCODE_INDEX] = BOOTLOADER_NACK_OPCODE;
          }
        } else {
          tx_cmd_buff_o->data[MSG_LEN_INDEX] = ((uint8_t)0x06);
          tx_cmd_buff_o->data[OPCODE_INDEX] = COMMON_NACK_OPCODE;
        }
        break;
      case APP_SET_TIME_OPCODE:
        // collect bytes
        sec_0 = rx_cmd_buff_o->data[PLD_START_INDEX+0]; // LSB
        sec_1 = rx_cmd_buff_o->data[PLD_START_INDEX+1];
        sec_2 = rx_cmd_buff_o->data[PLD_START_INDEX+2];
        sec_3 = rx_cmd_buff_o->data[PLD_START_INDEX+3]; // MSB
        ns_0  = rx_cmd_buff_o->data[PLD_START_INDEX+4]; // LSB
        ns_1  = rx_cmd_buff_o->data[PLD_START_INDEX+5];
        ns_2  = rx_cmd_buff_o->data[PLD_START_INDEX+6];
        ns_3  = rx_cmd_buff_o->data[PLD_START_INDEX+7]; // MSB
        // initialize common variables to known values
        sec = 0;
        ns  = 0;
        // assemble bytes
        sec = (uint32_t)((sec_3<<24) | (sec_2<<16) | (sec_1<<8) | (sec_0<<0));
        ns  = (uint32_t)(( ns_3<<24) | ( ns_2<<16) | ( ns_1<<8) | ( ns_0<<0));
        // use assembled bytes
        success = handle_app_set_time(sec, ns);
        // reply
        if(success) {
          tx_cmd_buff_o->data[MSG_LEN_INDEX] = ((uint8_t)0x06);
          tx_cmd_buff_o->data[OPCODE_INDEX] = COMMON_ACK_OPCODE;
        } else {
          tx_cmd_buff_o->data[MSG_LEN_INDEX] = ((uint8_t)0x06);
          tx_cmd_buff_o->data[OPCODE_INDEX] = COMMON_NACK_OPCODE;
        }
        break;
      case APP_GET_TIME_OPCODE:
        // initialize common variables to known values
        sec     = 0;
        ns      = 0;
        success = 0;
        success = handle_app_get_time(&sec, &ns);
        if(success) {
          tx_cmd_buff_o->data[MSG_LEN_INDEX] = ((uint8_t)0x0e);
          tx_cmd_buff_o->data[OPCODE_INDEX] = APP_SET_TIME_OPCODE;
          sec_0 = (sec >>  0) & 0xff; // LSB
          sec_1 = (sec >>  8) & 0xff;
          sec_2 = (sec >> 16) & 0xff;
          sec_3 = (sec >> 24) & 0xff; // MSB
          ns_0  = (ns  >>  0) & 0xff; // LSB
          ns_1  = (ns  >>  8) & 0xff;
          ns_2  = (ns  >> 16) & 0xff;
          ns_3  = (ns  >> 24) & 0xff; // MSB
          tx_cmd_buff_o->data[PLD_START_INDEX+0] = sec_0;
          tx_cmd_buff_o->data[PLD_START_INDEX+1] = sec_1;
          tx_cmd_buff_o->data[PLD_START_INDEX+2] = sec_2;
          tx_cmd_buff_o->data[PLD_START_INDEX+3] = sec_3;
          tx_cmd_buff_o->data[PLD_START_INDEX+4] =  ns_0;
          tx_cmd_buff_o->data[PLD_START_INDEX+5] =  ns_1;
          tx_cmd_buff_o->data[PLD_START_INDEX+6] =  ns_2;
          tx_cmd_buff_o->data[PLD_START_INDEX+7] =  ns_3;
        } else {
          tx_cmd_buff_o->data[MSG_LEN_INDEX] = ((uint8_t)0x06);
          tx_cmd_buff_o->data[OPCODE_INDEX] = COMMON_NACK_OPCODE;
        }
        break;
      default:
        break;
    }
    tx_cmd_buff_o->end_index = // +((uint8_t)0x03) accounts for 1st 3 bytes
     (tx_cmd_buff_o->data[MSG_LEN_INDEX]+((uint8_t)0x03));
    tx_cmd_buff_o->empty = 0;
    clear_rx_cmd_buff(rx_cmd_buff_o);
  }
}

//// Attempts to pop byte from beginning of tx_cmd_buff
uint8_t pop_tx_cmd_buff(tx_cmd_buff_t* tx_cmd_buff_o) {
  uint8_t b = 0;
  if(
   !tx_cmd_buff_o->empty &&
   tx_cmd_buff_o->start_index<tx_cmd_buff_o->end_index
  ) {
    b = tx_cmd_buff_o->data[tx_cmd_buff_o->start_index];
    tx_cmd_buff_o->start_index += 1;
  }
  if(tx_cmd_buff_o->start_index==tx_cmd_buff_o->end_index) {
    clear_tx_cmd_buff(tx_cmd_buff_o);
  }
  return b;
}

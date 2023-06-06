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
extern int bootloader_active(void);

extern int get_dst_buff_index(uint8_t route_nibble, bool outbound);
extern uint16_t get_stack_hwid(void);
extern uint8_t get_route_id(void);

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
      default:
        break;
    }
    tx_cmd_buff_o->end_index = // +((uint8_t)0x03) accounts for 1st 3 bytes
     (tx_cmd_buff_o->data[MSG_LEN_INDEX]+((uint8_t)0x03));
    tx_cmd_buff_o->empty = 0;
    clear_rx_cmd_buff(rx_cmd_buff_o);
  }
}

void write_forward(rx_cmd_buff_t* rx_cmd_buff_o, tx_cmd_buff_t* tx_cmd_buff_o) {
  if(
   rx_cmd_buff_o->state==RX_CMD_BUFF_STATE_COMPLETE &&
   tx_cmd_buff_o->empty
  ) {
    tx_cmd_buff_o->data[START_BYTE_0_INDEX] = START_BYTE_0;
    tx_cmd_buff_o->data[START_BYTE_1_INDEX] = START_BYTE_1;
    tx_cmd_buff_o->data[MSG_LEN_INDEX] = rx_cmd_buff_o->data[MSG_LEN_INDEX];
    tx_cmd_buff_o->data[HWID_LSB_INDEX] = rx_cmd_buff_o->data[HWID_LSB_INDEX];
    tx_cmd_buff_o->data[HWID_MSB_INDEX] = rx_cmd_buff_o->data[HWID_MSB_INDEX];
    tx_cmd_buff_o->data[MSG_ID_LSB_INDEX] =
     rx_cmd_buff_o->data[MSG_ID_LSB_INDEX];
    tx_cmd_buff_o->data[MSG_ID_MSB_INDEX] =
     rx_cmd_buff_o->data[MSG_ID_MSB_INDEX];
    tx_cmd_buff_o->data[ROUTE_INDEX] =
     rx_cmd_buff_o->data[ROUTE_INDEX];
    tx_cmd_buff_o->data[OPCODE_INDEX] =
     rx_cmd_buff_o->data[OPCODE_INDEX];

    tx_cmd_buff_o->empty = 0;
    tx_cmd_buff_o->start_index = 0;
    tx_cmd_buff_o->end_index = PLD_START_INDEX;
    for (int i = 0; i < rx_cmd_buff_o->data[MSG_LEN_INDEX] - 6; i++) {
      tx_cmd_buff_o->data[PLD_START_INDEX + i] = rx_cmd_buff_o->data[PLD_START_INDEX + i];
      tx_cmd_buff_o->end_index++;
    }

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

uint16_t get_cmd_hwid(const rx_cmd_buff_t* const rx_cmd_buff_i) {
  return ((uint16_t)rx_cmd_buff_i->data[HWID_MSB_INDEX] << 8) + rx_cmd_buff_i->data[HWID_LSB_INDEX];
}

uint16_t get_cmd_msg_id(const rx_cmd_buff_t* const rx_cmd_buff_i) {
  return ((uint16_t)rx_cmd_buff_i->data[MSG_ID_MSB_INDEX] << 8) + rx_cmd_buff_i->data[MSG_ID_LSB_INDEX];
}

// Command route_index is src:dst, both are nibbles
uint8_t get_cmd_src(const rx_cmd_buff_t* const rx_cmd_buff_i) {
  return (rx_cmd_buff_i->data[ROUTE_INDEX] >> 4) & 0x0f;
}

uint8_t get_cmd_dst(const rx_cmd_buff_t* const rx_cmd_buff_i) {
  return (rx_cmd_buff_i->data[ROUTE_INDEX] >> 0) & 0x0f;
}

void add_live(liveness_accountant_t *accountant, uint16_t msg_id) {
  for (size_t i = 0; i < accountant->size; i++) {
    if (accountant->data[i] == 0) {
      accountant->data[i] = msg_id;
      return;
    }
  }
  // TODO: reaching here should raise an error
}

void remove_live(liveness_accountant_t *accountant, uint16_t msg_id) {
  for (size_t i = 0; i < accountant->size; i++) {
    if (accountant->data[i] == msg_id) {
      accountant->data[i] = 0;
    }
  }
}

bool is_live(liveness_accountant_t *accountant, uint16_t msg_id) {
  for (size_t i = 0; i < accountant->size; i++) {
    if (accountant->data[i] == msg_id) {
      return true;
    }
  }
  return false;
}

void route(rx_cmd_buff_t* rx_cmd_buff_o, liveness_accountant_t* accountant, size_t len_tx_cmd_buffs_o, tx_cmd_buff_t** tx_cmd_buffs_o) {
   // TODO: use to check result from get_dst_buff_index to prevent out of bounds write
  (void) len_tx_cmd_buffs_o;

  uint16_t msg_id = get_cmd_msg_id(rx_cmd_buff_o);
  bool live = is_live(accountant, msg_id);
  bool hwid_match = get_stack_hwid() == get_cmd_hwid(rx_cmd_buff_o);

  uint8_t src = get_cmd_src(rx_cmd_buff_o);
  uint8_t dst = get_cmd_dst(rx_cmd_buff_o);

  // clear commands that are not replies and do not match the hwid if they came from outside the board
  if (!live && !hwid_match && rx_cmd_buff_o->handles_offboard) {
    clear_rx_cmd_buff(rx_cmd_buff_o);
    return;
  }

  // sort buffer between inbound/outbound and command/reply
  if (!live && hwid_match) { // inbound command
    add_live(accountant, msg_id);
    if (dst == get_route_id()) { // correct board, generate reply
      write_reply(rx_cmd_buff_o, tx_cmd_buffs_o[get_dst_buff_index(rx_cmd_buff_o->reply_nibble, rx_cmd_buff_o->handles_offboard)]);
    } else { // forward
      write_forward(rx_cmd_buff_o, tx_cmd_buffs_o[get_dst_buff_index(dst, false)]);
    }
  } else if (!live && !hwid_match) { // outbound command
    add_live(accountant, msg_id);
    write_forward(rx_cmd_buff_o, tx_cmd_buffs_o[get_dst_buff_index(dst, true)]);
  } else if (live && !hwid_match) { // outbound reply
    remove_live(accountant, msg_id);
    write_forward(rx_cmd_buff_o, tx_cmd_buffs_o[get_dst_buff_index(src, true)]);
  } else { // inbound reply
    remove_live(accountant, msg_id);
    write_forward(rx_cmd_buff_o, tx_cmd_buffs_o[get_dst_buff_index(src, false)]);
  }
}
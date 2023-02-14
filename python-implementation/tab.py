# tab.py
#
# Python classes, functions, and variables supporting usage of TAB
#
# Written by Bradley Denby
# Other contributors: Chad Taylor
#
# See the top-level LICENSE file for the license.

# import Python modules
import enum # Enum

# "constants"

## General
CMD_MAX_LEN  = 258
PLD_MAX_LEN  = 249
START_BYTE_0 = 0x22
START_BYTE_1 = 0x69

## Opcodes
COMMON_ACK_OPCODE       = 0x10
COMMON_NACK_OPCODE      = 0xff
COMMON_DEBUG_OPCODE     = 0x11
COMMON_DATA_OPCODE      = 0x16
BOOTLOADER_ACK_OPCODE   = 0x01
BOOTLOADER_NACK_OPCODE  = 0x0f
BOOTLOADER_PING_OPCODE  = 0x00

## Route Nibble IDs
GND = 0x00
COM = 0x01
CDH = 0x02
PLD = 0x03

## TAB command indices
START_BYTE_0_INDEX = 0
START_BYTE_1_INDEX = 1
MSG_LEN_INDEX      = 2
HWID_LSB_INDEX     = 3
HWID_MSB_INDEX     = 4
MSG_ID_LSB_INDEX   = 5
MSG_ID_MSB_INDEX   = 6
ROUTE_INDEX        = 7
OPCODE_INDEX       = 8
PLD_START_INDEX    = 9

## TAOLST Command Enum Parameters
BOOTLOADER_ACK_REASON_PONG   = 0x00
BOOTLOADER_ACK_REASON_ERASED = 0x01
BOOTLOADER_ACK_REASON_JUMP   = 0xff

## Route nodes
class Route(enum.Enum):
  SRC = 0
  DST = 1

## RX command buffer states
class RxCmdBuffState(enum.Enum):
  START_BYTE_0 = 0x00
  START_BYTE_1 = 0x01
  MSG_LEN      = 0x02
  HWID_LSB     = 0x03
  HWID_MSB     = 0x04
  MSG_ID_LSB   = 0x05
  MSG_ID_MSB   = 0x06
  ROUTE        = 0x07
  OPCODE       = 0x08
  PLD          = 0x09
  COMPLETE     = 0x0a

## Common Data buffer
class CommonDataBuff:
  def __init__(self):
    self.end_index = 0
    self.data = [0x00]*PLD_MAX_LEN

  def clear(self):
    self.end_index = 0
    self.data = [0x00]*PLD_MAX_LEN

## CommonDataBuff instantiation
common_data_buff = CommonDataBuff()

## CommonDataBuff handler
def handle_common_data(common_data_buff):
  return False

## RX command buffer
class RxCmdBuff:
  def __init__(self):
    self.state = RxCmdBuffState.START_BYTE_0
    self.start_index = 0
    self.end_index = 0
    self.data = [0x00]*CMD_MAX_LEN

  def clear(self):
    self.state = RxCmdBuffState.START_BYTE_0
    self.start_index = 0
    self.end_index = 0
    self.data = [0x00]*CMD_MAX_LEN

  def append_byte(self, b):
    if self.state == RxCmdBuffState.START_BYTE_0:
      if b==START_BYTE_0:
        self.data[START_BYTE_0_INDEX] = b
        self.state = RxCmdBuffState.START_BYTE_1
    elif self.state == RxCmdBuffState.START_BYTE_1:
      if b==START_BYTE_1:
        self.data[START_BYTE_1_INDEX] = b
        self.state = RxCmdBuffState.MSG_LEN
      else:
        self.clear()
    elif self.state == RxCmdBuffState.MSG_LEN:
      if 0x06 <= b and b <= 0xff:
        self.data[MSG_LEN_INDEX] = b
        self.start_index = 0x09
        self.end_index = b+0x03
        self.state = RxCmdBuffState.HWID_LSB
      else:
        self.clear()
    elif self.state == RxCmdBuffState.HWID_LSB:
      self.data[HWID_LSB_INDEX] = b
      self.state = RxCmdBuffState.HWID_MSB
    elif self.state == RxCmdBuffState.HWID_MSB:
      self.data[HWID_MSB_INDEX] = b
      self.state = RxCmdBuffState.MSG_ID_LSB
    elif self.state == RxCmdBuffState.MSG_ID_LSB:
      self.data[MSG_ID_LSB_INDEX] = b
      self.state = RxCmdBuffState.MSG_ID_MSB
    elif self.state == RxCmdBuffState.MSG_ID_MSB:
      self.data[MSG_ID_MSB_INDEX] = b
      self.state = RxCmdBuffState.ROUTE
    elif self.state == RxCmdBuffState.ROUTE:
      self.data[ROUTE_INDEX] = b
      self.state = RxCmdBuffState.OPCODE
    elif self.state == RxCmdBuffState.OPCODE:
      self.data[OPCODE_INDEX] = b
      if self.start_index < self.end_index:
        self.state = RxCmdBuffState.PLD
      else:
        self.state = RxCmdBuffState.COMPLETE
    elif self.state == RxCmdBuffState.PLD:
      if self.start_index < self.end_index:
        self.data[self.start_index] = b
        self.start_index += 1
      if self.start_index == self.end_index:
        self.state = RxCmdBuffState.COMPLETE
    elif self.state == RxCmdBuffState.COMPLETE:
      pass

  def __str__(self):
    if self.state == RxCmdBuffState.COMPLETE:
      return cmd_bytes_to_str(self.data)
    else:
      pass

## TX command buffer
class TxCmdBuff:
  def __init__(self):
    self.empty = True
    self.start_index = 0
    self.end_index = 0
    self.data = [0x00]*CMD_MAX_LEN

  def clear(self):
    self.empty = True
    self.start_index = 0
    self.end_index = 0
    self.data = [0x00]*CMD_MAX_LEN

  def generate_reply(self, rx_cmd_buff):
    if rx_cmd_buff.state==RxCmdBuffState.COMPLETE and self.empty:
      self.data[START_BYTE_0_INDEX] = START_BYTE_0
      self.data[START_BYTE_1_INDEX] = START_BYTE_1
      self.data[HWID_LSB_INDEX] = rx_cmd_buff.data[HWID_LSB_INDEX]
      self.data[HWID_MSB_INDEX] = rx_cmd_buff.data[HWID_MSB_INDEX]
      self.data[MSG_ID_LSB_INDEX] = rx_cmd_buff.data[MSG_ID_LSB_INDEX]
      self.data[MSG_ID_MSB_INDEX] = rx_cmd_buff.data[MSG_ID_MSB_INDEX]
      self.data[ROUTE_INDEX] = \
       (0x0f & rx_cmd_buff.data[ROUTE_INDEX]) << 4 | \
       (0xf0 & rx_cmd_buff.data[ROUTE_INDEX]) >> 4;
      if rx_cmd_buff.data[OPCODE_INDEX] == COMMON_ACK_OPCODE:
        self.data[MSG_LEN_INDEX] = 0x06
        self.data[OPCODE_INDEX] = COMMON_ACK_OPCODE
      elif rx_cmd_buff.data[OPCODE_INDEX] == COMMON_NACK_OPCODE:
        self.data[MSG_LEN_INDEX] = 0x06
        self.data[OPCODE_INDEX] = COMMON_NACK_OPCODE
      elif rx_cmd_buff.data[OPCODE_INDEX] == COMMON_DEBUG_OPCODE:
        self.data[MSG_LEN_INDEX] = rx_cmd_buff.data[MSG_LEN_INDEX]
        self.data[OPCODE_INDEX] = COMMON_DEBUG_OPCODE
        for i in range(PLD_START_INDEX,rx_cmd_buff.end_index):
          self.data[i] = rx_cmd_buff.data[i]
      elif rx_cmd_buff.data[OPCODE_INDEX] == COMMON_DATA_OPCODE:
        # handle common data
        for i in range(PLD_START_INDEX,rx_cmd_buff.end_index):
          common_data_buff.data[i-PLD_START_INDEX] = rx_cmd_buff.data[i]
        common_data_buff.end_index = rx_cmd_buff.end_index-PLD_START_INDEX
        success = handle_common_data(common_data_buff)
        # reply
        if success:
          self.data[MSG_LEN_INDEX] = 0x06
          self.data[OPCODE_INDEX] = COMMON_ACK_OPCODE
        else:
          self.data[MSG_LEN_INDEX] = 0x06
          self.data[OPCODE_INDEX] = COMMON_NACK_OPCODE
      elif rx_cmd_buff.data[OPCODE_INDEX] == BOOTLOADER_ACK_OPCODE:
        self.data[MSG_LEN_INDEX] = 0x06
        self.data[OPCODE_INDEX] = COMMON_NACK_OPCODE
      elif rx_cmd_buff.data[OPCODE_INDEX] == BOOTLOADER_NACK_OPCODE:
        self.data[MSG_LEN_INDEX] = 0x06
        self.data[OPCODE_INDEX] = COMMON_NACK_OPCODE
      elif rx_cmd_buff.data[OPCODE_INDEX] == BOOTLOADER_PING_OPCODE:
        self.data[MSG_LEN_INDEX] = 0x06
        self.data[OPCODE_INDEX] = COMMON_NACK_OPCODE

# Helper functions

## Converts BOOTLOADER_ACK_REASON to string
def bootloader_ack_reason_to_str(bootloader_ack_reason):
  if bootloader_ack_reason==BOOTLOADER_ACK_REASON_PONG:
    return 'pong'
  elif bootloader_ack_reason==BOOTLOADER_ACK_REASON_ERASED:
    return 'erased'
  elif bootloader_ack_reason==BOOTLOADER_ACK_REASON_JUMP:
    return 'jump'
  else:
    return 'page number'

## Converts ROUTE byte to string (either SRC or DST as specified)
##   route: the TAB route byte, e.g. data[ROUTE_INDEX]
##   node: a Route node enum, i.e. Route.SRC or Route.DST
def route_to_str(route,node):
  # isolate the route nibble
  nibble = (route>>0) & 0x0f
  if node==Route.SRC:
    nibble = (route>>4) & 0x0f
  # return the corresponding string
  if nibble==GND:
    return 'gnd'
  elif nibble==COM:
    return 'com'
  elif nibble==CDH:
    return 'cdh'
  elif nibble==PLD:
    return 'pld'
  else:
    return '???'

## Converts a list of command bytes (ints) to a human-readable string
##   data: a list of 8-bit, unsigned integers
def cmd_bytes_to_str(data):
  # initialize command and payload strings
  cmd_str = ''
  pld_str = ''
  # command-specific string construction
  if data[OPCODE_INDEX] == COMMON_ACK_OPCODE:
    cmd_str += 'common_ack'
  elif data[OPCODE_INDEX] == COMMON_NACK_OPCODE:
    cmd_str += 'common_nack'
  elif data[OPCODE_INDEX] == COMMON_DEBUG_OPCODE:
    cmd_str += 'common_debug'
    pld_str = ' "'
    for i in range(0,data[MSG_LEN_INDEX]-0x06):
      pld_str += chr(data[PLD_START_INDEX+i])
    pld_str += '"'
  elif data[OPCODE_INDEX] == COMMON_DATA_OPCODE:
    cmd_str += 'common_data'
    for i in range(0,data[MSG_LEN_INDEX]-0x06):
      pld_str += ' 0x{:02x}'.format(data[PLD_START_INDEX+i])
  elif data[OPCODE_INDEX] == BOOTLOADER_ACK_OPCODE:
    cmd_str += 'bootloader_ack'
    if (data[MSG_LEN_INDEX] == 0x07):
      pld_str += ' reason:'+'0x{:02x}'.format(data[PLD_START_INDEX])+\
       '('+bootloader_ack_reason_to_str(data[PLD_START_INDEX])+')'
  elif data[OPCODE_INDEX] == BOOTLOADER_NACK_OPCODE:
    cmd_str += 'bootloader_nack'
  elif data[OPCODE_INDEX] == BOOTLOADER_PING_OPCODE:
    cmd_str += 'bootloader_ping'
  # string construction common to all commands
  cmd_str += ' hw_id:0x{:04x}'.format(\
   (data[HWID_MSB_INDEX]<<8)|(data[HWID_LSB_INDEX]<<0)\
  )
  cmd_str += ' msg_id:0x{:04x}'.format(\
   (data[MSG_ID_MSB_INDEX]<<8)|(data[MSG_ID_LSB_INDEX]<<0)\
  )
  cmd_str += ' src:'+route_to_str(data[ROUTE_INDEX],Route.SRC)
  cmd_str += ' dst:'+route_to_str(data[ROUTE_INDEX],Route.DST)
  return (cmd_str+pld_str)

## A Python class for easily constructing commands to be placed in a TX buffer
##   TODO: a "valid" state variable indicating whether data is a valid command
##   This valid state variable is important for commands with payloads that are
##   constructed in two steps, because between the two steps they can have an
##   invalid message length (i.e. length 6 when the payload must be at least 1)
class TxCmd:
  def __init__(self, opcode, hw_id, msg_id, src, dst):
    # set up data buffer
    self.data = [0x00]*CMD_MAX_LEN
    # set command header bytes
    self.data[START_BYTE_0_INDEX] = START_BYTE_0
    self.data[START_BYTE_1_INDEX] = START_BYTE_1
    self.data[HWID_LSB_INDEX]     = (hw_id  >> 0) & 0xff
    self.data[HWID_MSB_INDEX]     = (hw_id  >> 8) & 0xff
    self.data[MSG_ID_LSB_INDEX]   = (msg_id >> 0) & 0xff
    self.data[MSG_ID_MSB_INDEX]   = (msg_id >> 8) & 0xff
    self.data[ROUTE_INDEX]        = (src << 4) | (dst << 0)
    self.data[OPCODE_INDEX]       = opcode
    # set opcode-specific bytes
    if self.data[OPCODE_INDEX] == COMMON_ACK_OPCODE:
      self.data[MSG_LEN_INDEX] = 0x06
    elif self.data[OPCODE_INDEX] == COMMON_NACK_OPCODE:
      self.data[MSG_LEN_INDEX] = 0x06
    elif self.data[OPCODE_INDEX] == COMMON_DEBUG_OPCODE:
      self.data[MSG_LEN_INDEX] = 0x06
    elif self.data[OPCODE_INDEX] == COMMON_DATA_OPCODE:
      self.data[MSG_LEN_INDEX] = 0x06
    elif self.data[OPCODE_INDEX] == BOOTLOADER_ACK_OPCODE:
      self.data[MSG_LEN_INDEX] = 0x06
    elif self.data[OPCODE_INDEX] == BOOTLOADER_NACK_OPCODE:
      self.data[MSG_LEN_INDEX] = 0x06
    elif self.data[OPCODE_INDEX] == BOOTLOADER_PING_OPCODE:
      self.data[MSG_LEN_INDEX] = 0x06
    else:
      self.data[MSG_LEN_INDEX] = 0x06

  def common_debug(self, ascii):
    if self.data[OPCODE_INDEX] == COMMON_DEBUG_OPCODE:
      if len(ascii)<=PLD_MAX_LEN:
        self.data[MSG_LEN_INDEX] = 0x06+len(ascii)
        for i in range(0,len(ascii)):
          self.data[PLD_START_INDEX+i] = ord(ascii[i])

  def common_data(self, bytes):
    if self.data[OPCODE_INDEX] == COMMON_DATA_OPCODE:
      if len(bytes)<=PLD_MAX_LEN:
        self.data[MSG_LEN_INDEX] = 0x06+len(bytes)
        for i in range(0,len(bytes)):
          self.data[PLD_START_INDEX+i] = bytes[i]

  def get_byte_count(self):
    return self.data[MSG_LEN_INDEX]+0x03

  def clear(self):
    self.data = [0x00]*CMD_MAX_LEN

  def __str__(self):
    return cmd_bytes_to_str(self.data)

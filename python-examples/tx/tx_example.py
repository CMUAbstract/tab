# Usage: python3 tx_example.py /path/to/dev
# Parameters:
#  /path/to/dev: path to device, e.g. /dev/ttyUSB0
# Output:
#  Prints results to the command line

# import Python modules
import os     # path
import serial # serial
import sys    # accessing script arguments
import time   # sleep

# Make Python TAB implementation visible
sys.path.append(os.path.abspath('../../python-implementation/'))

# import TAB support
from tab import *

################################################################################

# initialize script arguments
dev = '' # serial device

# parse script arguments
if len(sys.argv)==2:
  dev = sys.argv[1]
else:
  print(\
   'Usage: '\
   'python3 tx_example.py '\
   '/path/to/dev'\
  )
  exit()

# Create serial object
try:
  serial_port = serial.Serial(port=dev,baudrate=115200)
except:
  print('Serial port object creation failed:')
  print('  '+dev)
  exit()

################################################################################

# Set up test support variables
msgid = 0x0000
rx_cmd_buff = RxCmdBuff()

# 1. Basic test
for i in range(0,5):
  cmd = TxCmd(COMMON_ACK_OPCODE, HWID, msgid, SRC, DST)
  byte_i = 0
  while rx_cmd_buff.state != RxCmdBuffState.COMPLETE:
    if byte_i < cmd.get_byte_count():
      serial_port.write(cmd.data[byte_i].to_bytes(1, byteorder='big'))
      byte_i += 1
    if serial_port.in_waiting>0:
      bytes = serial_port.read(1)
      for b in bytes:
        rx_cmd_buff.append_byte(b)
  print('txcmd: '+str(cmd))
  print('reply: '+str(rx_cmd_buff)+'\n')
  cmd.clear()
  rx_cmd_buff.clear()
  msgid += 1
  time.sleep(1.0)

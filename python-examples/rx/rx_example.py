# Usage: python3 rx_example.py /path/to/dev
# Parameters:
#  /path/to/dev: path to device, e.g. /dev/ttyUSB0
# Output:
#  Prints results to the command line

# import Python modules
import serial # serial
import sys    # accessing script arguments

# Make Python TAB implementation visible
sys.path.append(os.path.abspath('../python-implementation/'))

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
   'python3 rx_example.py '\
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
rx_cmd_buff = RxCmdBuff()
tx_cmd_buff = TxCmdBuff()

# Listen and reply
cmd_count = 0
while cmd_count<12:
  while rx_cmd_buff.state != RxCmdBuffState.COMPLETE:
    if serial_port.in_waiting>0:
      bytes = serial_port.read(1)
      for b in bytes:
        rx_cmd_buff.append_byte(b)
  tx_cmd_buff.generate_reply(rx_cmd_buff)
  byte_i = 0
  while byte_i < tx_cmd_buff.get_byte_count():
    serial_port.write(tx_cmd_buff.data[byte_i].to_bytes(1, byteorder='big'))
    byte_i += 1
  print('rxcmd: '+str(rx_cmd_buff))
  print('reply: '+str(tx_cmd_buff)+'\n')
  rx_cmd_buff.clear()
  tx_cmd_buff.clear()
  cmd_count += 1

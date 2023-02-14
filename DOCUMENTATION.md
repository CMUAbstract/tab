# TAB Documentation

If you are interested in using and understanding TAB, refer to this document.

## Overview

* [Commands](#commands): TAB commands
  * [Common Ack](#common-ack)
  * [Common Nack](#common-nack)
  * [Common Debug](#common-debug)
  * [Common Data](#common-data)
* [Protocol](#protocol): TAB protocol
* [License](#license)

## <a name="commands"></a> Commands

TAB consists of a small number of commands. These commands are documentated in
the following subsections.

### <a name="common-ack"></a> Common Ack

This acknowledgement command is most often used as a reply indicating success.
It is also useful as a command to check whether the recipient is "alive."
* Name: `common_ack`
* Required parameters: None
* Optional parameters: None
* Reply: `common_ack`

**Header**

| Start Byte 0 | Start Byte 1 | Remaining Bytes | HW ID LSByte | HW ID MSByte | MSG ID LSByte | MSG ID MSByte | Route Nibbles | Opcode |
| ------------ | ------------ | --------------- | ------------ | ------------ | ------------- | ------------- | ------------- | ------ |
| 0x22         | 0x69         | 0x06            | 0xHH         | 0xHH         | 0xHH          | 0xHH          | 0xSD          | 0x10   |

**Payload**

No payload

### <a name="common-nack"></a> Common Nack

This negative-acknowledgement command is used as a reply indicating failure.
* Name: `common_nack`
* Required parameters: None
* Optional parameters: None
* Reply: `common_nack`
  * Because this command is used as a reply, sending this command generates a
    common nack reply.

**Header**

| Start Byte 0 | Start Byte 1 | Remaining Bytes | HW ID LSByte | HW ID MSByte | MSG ID LSByte | MSG ID MSByte | Route Nibbles | Opcode |
| ------------ | ------------ | --------------- | ------------ | ------------ | ------------- | ------------- | ------------- | ------ |
| 0x22         | 0x69         | 0x06            | 0xHH         | 0xHH         | 0xHH          | 0xHH          | 0xSD          | 0xff   |

**Payload**

No payload

### <a name="common-debug"></a> Common Debug

This command supports a variable-length ASCII payload useful for debugging.
* Name: `common_debug`
* Required parameters: A sequence of one or more ASCII characters (not null
  terminated)
* Optional parameters: None
* Reply: `common_debug` with the original payload of ASCII characters

**Header**

| Start Byte 0 | Start Byte 1 | Remaining Bytes | HW ID LSByte | HW ID MSByte | MSG ID LSByte | MSG ID MSByte | Route Nibbles | Opcode |
| ------------ | ------------ | --------------- | ------------ | ------------ | ------------- | ------------- | ------------- | ------ |
| 0x22         | 0x69         | 0xHH            | 0xHH         | 0xHH         | 0xHH          | 0xHH          | 0xSD          | 0x11   |

* Remaining Bytes must be greater than or equal to 0x07

**Payload**

| ASCII character(s) |
| ------------------ |
| 0xHH               |

* Up to 249 ASCII characters (not null terminated)

### <a name="common-data"></a> Common Data

This command supports a variable-length byte payload useful for data transfer.
* Name: `common_data`
* Required parameters: A sequence of one or more bytes
* Optional parameters: None
* Reply:
  * If the byte payload handler is successful: `common_ack`
  * Otherwise: `common_nack`

**Header**

| Start Byte 0 | Start Byte 1 | Remaining Bytes | HW ID LSByte | HW ID MSByte | MSG ID LSByte | MSG ID MSByte | Route Nibbles | Opcode |
| ------------ | ------------ | --------------- | ------------ | ------------ | ------------- | ------------- | ------------- | ------ |
| 0x22         | 0x69         | 0xHH            | 0xHH         | 0xHH         | 0xHH          | 0xHH          | 0xSD          | 0x16   |

* Remaining Bytes must be greater than or equal to 0x07

**Payload**

| Byte(s) |
| ------- |
| 0xHH    |

* Up to 249 bytes

### <a name="bootloader-ack"></a> Bootloader Ack

This acknowledgement command is most often used as a reply indicating success while the board is in bootloader mode for specific bootloader commands.
* Name: `bootloader_ack`
* Required parameters: None
* Optional parameters: Single byte response reason parameter
* Reply: `common_nack`
  * Because this command is used as a reply, sending this command generates a
    common nack reply.

**Header**

| Start Byte 0 | Start Byte 1 | Remaining Bytes | HW ID LSByte | HW ID MSByte | MSG ID LSByte | MSG ID MSByte | Route Nibbles | Opcode |
| ------------ | ------------ | --------------- | ------------ | ------------ | ------------- | ------------- | ------------- | ------ |
| 0x22         | 0x69         | 0x06 OR 0x07    | 0xHH         | 0xHH         | 0xHH          | 0xHH          | 0xSD          | 0x01   |

**Optional Payload**

| Byte    |
| ------- |
| 0xHH    |

Reason:
* PONG (response to ping): 0x00
* ERASED (response to erase): 0x01
* JUMP (response to jump): 0xFF
* Page Number (response to write page)

### <a name="bootloader-nack"></a> Bootloader Nack

This negative-acknowledgement command is used as a reply indicating failure while the board is in bootloader mode for specific bootloader commands.
* Name: `bootloader_nack`
* Required parameters: None
* Optional parameters: None
* Reply: `common_nack`
  * Because this command is used as a reply, sending this command generates a
    common nack reply.

**Header**

| Start Byte 0 | Start Byte 1 | Remaining Bytes | HW ID LSByte | HW ID MSByte | MSG ID LSByte | MSG ID MSByte | Route Nibbles | Opcode |
| ------------ | ------------ | --------------- | ------------ | ------------ | ------------- | ------------- | ------------- | ------ |
| 0x22         | 0x69         | 0x06            | 0xHH         | 0xHH         | 0xHH          | 0xHH          | 0xSD          | 0x0F   |

### <a name="bootloader-ping"></a> Bootloader Ping

This ping command is used to check if the board is alive and is in the boot mode.
* Name: `bootloader_ping`
* Required parameters: None
* Optional parameters: None
* Reply:
  * If board is in application mode and not bootloader mode: `common_nack`
  * If board is in bootloader mode and not application mode: `bootloader_ack` with PONG parameter

**Header**

| Start Byte 0 | Start Byte 1 | Remaining Bytes | HW ID LSByte | HW ID MSByte | MSG ID LSByte | MSG ID MSByte | Route Nibbles | Opcode |
| ------------ | ------------ | --------------- | ------------ | ------------ | ------------- | ------------- | ------------- | ------ |
| 0x22         | 0x69         | 0x06            | 0xHH         | 0xHH         | 0xHH          | 0xHH          | 0xSD          | 0x00   |


## <a name="protocol"></a> Protocol

TODO

## <a name="license"></a> License

Written by Bradley Denby  
Other contributors: Chad Taylor

See the top-level LICENSE file for the license.

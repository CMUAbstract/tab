# TAB Documentation

If you are interested in using and understanding TAB, refer to this document.

## Overview

* [Commands](#commands): TAB commands
  * [Common Ack](#common-ack)
  * [Common Nack](#common-nack)
  * [Common Debug](#common-debug)
  * [Common Data](#common-data)
  * [Bootloader Ack](#bootloader-ack)
  * [Bootloader Nack](#bootloader-nack)
  * [Bootloader Ping](#bootloader-ping)
  * [Bootlaoder Erase](#bootloader-erase)
  * [Bootloader Write Page](#bootloader-write-page)
  * [Bootloader Write Page Addr32](#bootloader-write-page-addr32)
  * [Bootloader Jump](#bootloader-jump)
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

The bootloader acknowledgement command is exclusively used as a bootloader reply
indicating success.
* Name: `bootloader_ack`
* Required parameters: None
* Optional parameters: Single byte response (reason parameter)
* Reply: `common_nack`
  * Because this command is used as a reply, sending this command generates a
    common nack reply.

**Header**

| Start Byte 0 | Start Byte 1 | Remaining Bytes      | HW ID LSByte | HW ID MSByte | MSG ID LSByte | MSG ID MSByte | Route Nibbles | Opcode |
| ------------ | ------------ | -------------------- | ------------ | ------------ | ------------- | ------------- | ------------- | ------ |
| 0x22         | 0x69         | 0x06 OR 0x07 OR 0x0a | 0xHH         | 0xHH         | 0xHH          | 0xHH          | 0xSD          | 0x01   |

**Optional Payload**

| Reason  |
| ------- |
| 0xHH    |

Reason:
* PONG (response to ping): 0x00
* ERASED (response to erase): 0x01
* JUMP (response to jump): 0xFF
* Page Number (response to write page)
* Address (response to write page addr32)

### <a name="bootloader-nack"></a> Bootloader Nack

The bootloader negative-acknowledgement command is used as a bootloader reply
indicating failure.
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

The bootloader ping command checks whether the bootloader is active.
* Name: `bootloader_ping`
* Required parameters: None
* Optional parameters: None
* Reply:
  * If the bootloader is active: `bootloader_ack` with the PONG payload
  * Otherwise: `common_nack`

**Header**

| Start Byte 0 | Start Byte 1 | Remaining Bytes | HW ID LSByte | HW ID MSByte | MSG ID LSByte | MSG ID MSByte | Route Nibbles | Opcode |
| ------------ | ------------ | --------------- | ------------ | ------------ | ------------- | ------------- | ------------- | ------ |
| 0x22         | 0x69         | 0x06            | 0xHH         | 0xHH         | 0xHH          | 0xHH          | 0xSD          | 0x00   |

### <a name="bootloader-erase"></a> Bootloader Erase

This command instructs the bootloader to erase all applications.
* Name: `bootloader_erase`
* Required parameters: None
* Optional parameters: Status
* Reply:
  * If the bootloader is active and successfully performs the erase:
    `bootloader_ack` with the ERASE payload
  * If the bootloader is active and fails to perform the erase:
    `bootloader_nack`
  * Otherwise: `common_nack`

**Header**

| Start Byte 0 | Start Byte 1 | Remaining Bytes | HW ID LSByte | HW ID MSByte | MSG ID LSByte | MSG ID MSByte | Route Nibbles | Opcode |
| ------------ | ------------ | --------------- | ------------ | ------------ | ------------- | ------------- | ------------- | ------ |
| 0x22         | 0x69         | 0x06            | 0xHH         | 0xHH         | 0xHH          | 0xHH          | 0xSD          | 0x0C   |

**Optional Payload**

| Status  |
| ------- |
| 0xHH    |

* The status parameter is currently unused.

### <a name="bootloader-write-page"></a> Bootloader Write Page

This command is used to write programs to the application memory space
of the board using the page number to calculate the address to write to.
* Name: `bootloader_write_page`
* Required parameters: Page Number
* Optional parameters: Page Data
* Reply:
  * If the bootloader is active and successfully performs the write:
    `bootloader_ack` with the Page Number as the payload
  * If the bootloader is active and fails to perform the write:
    `bootloader_nack`
  * Otherwise: `common_nack`

**Header**

| Start Byte 0 | Start Byte 1 | Remaining Bytes | HW ID LSByte | HW ID MSByte | MSG ID LSByte | MSG ID MSByte | Route Nibbles | Opcode |
| ------------ | ------------ | --------------- | ------------ | ------------ | ------------- | ------------- | ------------- | ------ |
| 0x22         | 0x69         | 0x07 OR 0x87    | 0xHH         | 0xHH         | 0xHH          | 0xHH          | 0xSD          | 0x02   |

**Optional Payload**

| Page Number  | Page Data | 
| ------------ | --------- |
| 0xHH         | 0xHH      |

* Page Data has 128 bytes.

### <a name="bootloader-write-page-addr32"></a> Bootloader Write Page Addr32

This command is used to write programs to the application memory space
of the board using the address provided.
* Name: `bootloader_write_page_addr32`
* Required parameters: Address
* Optional parameters: Page Data
* Reply:
  * If the bootloader is active and successfully performs the write:
    `bootloader_ack` with the Address as the payload
  * If the bootloader is active and fails to perform the write:
    `bootloader_nack`
  * Otherwise: `common_nack`

**Header**

| Start Byte 0 | Start Byte 1 | Remaining Bytes | HW ID LSByte | HW ID MSByte | MSG ID LSByte | MSG ID MSByte | Route Nibbles | Opcode |
| ------------ | ------------ | --------------- | ------------ | ------------ | ------------- | ------------- | ------------- | ------ |
| 0x22         | 0x69         | 0x0a OR 0x8a    | 0xHH         | 0xHH         | 0xHH          | 0xHH          | 0xSD          | 0x20   |

**Optional Payload**

| Addr Byte1 | Addr Byte2 | Addr Byte3 | Addr Byte4 | Page Data |
| ---------- | ---------- | ---------- | ---------- | --------- |
| 0xHH       | 0xHH       | 0xHH       | 0xHH       | 0xHH      |

* Addr Byte1 is the MSByte
* Addr Byte4 is the LSByte
* Page Data has 128 bytes.

### <a name="bootloader-jump"></a> Bootloader Jump

The command instructs the bootloader to jump to a different address in memory
* Name: `bootloader_jump`
* Required parameters: None
* Optional parameters: None
* Reply:
  * If the bootloader is active and successfully performs the jump:
    `bootloader_ack` with the JUMP payload
  * If the bootloader is active and fails to perform the jump:
    `bootloader_nack`
  * Otherwise: `common_nack`

**Header**

| Start Byte 0 | Start Byte 1 | Remaining Bytes | HW ID LSByte | HW ID MSByte | MSG ID LSByte | MSG ID MSByte | Route Nibbles | Opcode |
| ------------ | ------------ | --------------- | ------------ | ------------ | ------------- | ------------- | ------------- | ------ |
| 0x22         | 0x69         | 0x06            | 0xHH         | 0xHH         | 0xHH          | 0xHH          | 0xSD          | 0x0b   |

## <a name="protocol"></a> Protocol

TODO

## <a name="license"></a> License

Written by Bradley Denby  
Other contributors: Chad Taylor

See the top-level LICENSE file for the license.

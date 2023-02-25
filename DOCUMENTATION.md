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
* Optional parameters: Reason bytes
* Reply: `common_nack`
  * Because this command is used as a reply, sending this command generates a
    common nack reply.

**Header**

| Start Byte 0 | Start Byte 1 | Remaining Bytes      | HW ID LSByte | HW ID MSByte | MSG ID LSByte | MSG ID MSByte | Route Nibbles | Opcode |
| ------------ | ------------ | -------------------- | ------------ | ------------ | ------------- | ------------- | ------------- | ------ |
| 0x22         | 0x69         | 0x06 OR 0x07 OR 0x0a | 0xHH         | 0xHH         | 0xHH          | 0xHH          | 0xSD          | 0x01   |

**Payload**

| Reason (Optional) | Reason (Optional) | Reason (Optional) | Reason (Optional) |
| ----------------- | ----------------- | ----------------- | ----------------- |
| 0xHH              | 0xHH              | 0xHH              | 0xHH              |

Reason bytes:
* Remaining Bytes: 0x07
  * Response to `booloader_ping`: 0x00 (PONG)
  * Response to `bootloader_erase`: 0x01 (ERASED)
  * Response to `bootloader_jump`: 0xff (JUMPED)
  * Response to `bootloader_write_page`: 0xHH (Page Number)
* Remaining Bytes: 0x0a
  * Response to `bootloader_write_page_addr32`: 0xHH 0xHH 0xHH 0xHH (Start
    Address)

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
| 0x22         | 0x69         | 0x06            | 0xHH         | 0xHH         | 0xHH          | 0xHH          | 0xSD          | 0x0f   |

**Payload**

No payload

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

**Payload**

No payload

### <a name="bootloader-erase"></a> Bootloader Erase

This command instructs the bootloader to erase all applications.
* Name: `bootloader_erase`
* Required parameters: None
* Optional parameters: Status
* Reply:
  * If the bootloader is active and successfully performs the erase:
    `bootloader_ack` with the ERASED payload
  * If the bootloader is active and fails to perform the erase:
    `bootloader_nack`
  * Otherwise: `common_nack`

**Header**

| Start Byte 0 | Start Byte 1 | Remaining Bytes | HW ID LSByte | HW ID MSByte | MSG ID LSByte | MSG ID MSByte | Route Nibbles | Opcode |
| ------------ | ------------ | --------------- | ------------ | ------------ | ------------- | ------------- | ------------- | ------ |
| 0x22         | 0x69         | 0x06            | 0xHH         | 0xHH         | 0xHH          | 0xHH          | 0xSD          | 0x0c   |

**Payload**

| Status (Optional) |
| ----------------- |
| 0xHH              |

* The status parameter is currently unused.

### <a name="bootloader-write-page"></a> Bootloader Write Page

The bootloader write page command instructs the bootloader to write 128 bytes to
a 128-byte region of (presumably nonvolatile) memory indexed by the "page
number" parameter (the "sub-page ID"). For backwards-compatibility, the 128
bytes of "sub-page" data are optional (presumably, this feature allows for
"dry-runs" or page number validity probes). The "page number" parameter
("sub-page ID") is required.
* Name: `bootloader_write_page`
* Required parameters: Page Number (Sub-Page ID)
* Optional parameters: (Sub-)Page Data
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

**Payload**

| Page Number (Required) | Page Data (Optional) | 
| ---------------------- | -------------------- |
| 0xHH                   | 0xHH x128            |

* If present, the Page Data consists of 128 bytes.

### <a name="bootloader-write-page-addr32"></a> Bootloader Write Page Addr32

This command instructs the bootloader to write 128 bytes to a region of
(presumably nonvolatile) memory starting at the provided 32-bit address. The
target start address is required, and the 128 bytes of data are optional. This
feature allows for "dry-runs" or target start address validity probes.
* Name: `bootloader_write_page_addr32`
* Required parameters: Start Address
* Optional parameters: Sub-Page Data
* Reply:
  * If the bootloader is active and successfully performs the write:
    `bootloader_ack` with the Start Address as the payload
  * If the bootloader is active and fails to perform the write:
    `bootloader_nack`
  * Otherwise: `common_nack`

**Header**

| Start Byte 0 | Start Byte 1 | Remaining Bytes | HW ID LSByte | HW ID MSByte | MSG ID LSByte | MSG ID MSByte | Route Nibbles | Opcode |
| ------------ | ------------ | --------------- | ------------ | ------------ | ------------- | ------------- | ------------- | ------ |
| 0x22         | 0x69         | 0x0a OR 0x8a    | 0xHH         | 0xHH         | 0xHH          | 0xHH          | 0xSD          | 0x20   |

**Payload**

| Start Address MSByte (Required) | Start Address Byte 2 (Required) | Start Address Byte 3 (Required) | Start Address LSByte (Required) | Sub-Page Data (Optional) |
| ------------------------------- | ------------------------------- | ------------------------------- | ------------------------------- | ------------------------ |
| 0xHH                            | 0xHH                            | 0xHH                            | 0xHH                            | 0xHH x128                |

* If present, the Sub-Page Data consists of 128 bytes.

### <a name="bootloader-jump"></a> Bootloader Jump

The bootloader jump command instructs the bootloader to start a user
application.
* Name: `bootloader_jump`
* Required parameters: None
* Optional parameters: None
* Reply:
  * If the bootloader is active and successfully performs the jump:
    `bootloader_ack` with the JUMPED payload
  * If the bootloader is active and fails to perform the jump:
    `bootloader_nack`
  * Otherwise: `common_nack`

**Header**

| Start Byte 0 | Start Byte 1 | Remaining Bytes | HW ID LSByte | HW ID MSByte | MSG ID LSByte | MSG ID MSByte | Route Nibbles | Opcode |
| ------------ | ------------ | --------------- | ------------ | ------------ | ------------- | ------------- | ------------- | ------ |
| 0x22         | 0x69         | 0x06            | 0xHH         | 0xHH         | 0xHH          | 0xHH          | 0xSD          | 0x0b   |

**Payload**

No payload

## <a name="protocol"></a> Protocol

TODO

## <a name="license"></a> License

Written by Bradley Denby  
Other contributors: Chad Taylor

See the top-level LICENSE file for the license.

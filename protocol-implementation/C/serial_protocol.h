#ifndef SERIAL_PROTOCOL_H
#define SERIAL_PROTOCOL_H

/* Message format is the following:
- Payload length is unsigned 16-bit integer, denoted in the table as (n).
- Payload is n bytes long, where 0 is a valid value.
- Payload start represents the start of the payload if the payload is not empty.

| Byte pos | Title           | 
| -------- | --------------- |
| 0        | Header 0        |
| 1        | Header 1        |
| 2        | Header 2        |
| 3        | Src addr        |
| 4        | Tgt addr        |
| 5        | Msg type        |
| 6        | Payload len MSB |
| 7        | Payload len LSB |
| ...      | Payload start   |
| (8 + n)  | CRC16 MSB       |
| (9 + n)  | CRC16 LSB       |

*/

#include <stdint.h>

// Byte positions in the message.
#define HEADER_0_POS 0
#define HEADER_1_POS 1
#define HEADER_2_POS 2
#define SRC_ADDR_POS 3
#define TGT_ADDR_POS 4
#define MSG_TYPE_POS 5  
#define PAYLOAD_LEN_MSB_POS 6
#define PAYLOAD_LEN_LSB_POS 7

// Default header values - you can override these in your implementation.
#define HEADER_0_VAL 0xAA
#define HEADER_1_VAL 0x55
#define HEADER_2_VAL 0xFF

#endif /* SERIAL_PROTOCOL_H */
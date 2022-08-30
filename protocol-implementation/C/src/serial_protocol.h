#ifndef SERIAL_PROTOCOL_H
#define SERIAL_PROTOCOL_H

#ifdef __cplusplus
extern "C"
{
#endif

    /* Message format is the following:
    - Payload length is unsigned 16-bit integer, denoted in the table as (n).
    - Payload is n bytes long, where 0 is a valid value.
    - Payload start represents the start of the payload if the payload is not empty.
    - This library assumes all bytes are 8 bits in length, and truncates input to 8 bits.
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
#define PAYLOAD_START_POS 8

// Default header values - you can override these in your implementation.
#define HEADER_0_VAL 0xAA
#define HEADER_1_VAL 0x55
#define HEADER_2_VAL 0xFF

// Define the maximum allowable payload length. The maximum possible size is 65535 bytes.
#define MAX_PAYLOAD_LEN 65535

// Buffer length parameters
#define NUM_OVER_HEAD_BYTES 10
#define RX_BUFFER_LEN ((MAX_PAYLOAD_LEN + NUM_OVER_HEAD_BYTES) * 2)

// Device address - you need to override this in your implementation.
#define MY_ADDR 0x00

// If this device is the host on the bus this is where you can implement periphal functionality.
//#define PERIPHERAL_ADDR_0 0x01
//#define PERIPHERAL_ADDR_1 0x02

// Protocol version
#define PROTOCOL_VERSION 0x0001

// Error codes
#define SP_OK 0
#define SP_ERR_ILLEGAL_MSG_LEN -1
#define SP_ERR_NULL_PAYLOAD -2

    void parse_input_buffer(uint8_t *input_buffer, uint32_t max_length);
    int send_message(uint8_t dest_addr, uint8_t type, uint8_t *payload, uint32_t payload_length);

#ifdef __cplusplus
}
#endif

#endif /* SERIAL_PROTOCOL_H */
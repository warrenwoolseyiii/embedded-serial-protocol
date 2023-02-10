
/*DISCLAIMER:

This code is protected under the MIT open source license. The code is provided
"as is" without warranty of any kind, either express or implied, including but
not limited to the implied warranties of merchantability, fitness for a particular
purpose, or non-infringement. In no event shall the author or any other party be
liable for any direct, indirect, incidental, special, exemplary, or consequential
damages, however caused and on any theory of liability, whether in contract,
strict liability, or tort (including negligence or otherwise), arising in any way
out of the use of this code or performance or use of the results of this code. By
using this code, you agree to hold the author and any other party harmless from
any and all liability and to use the code at your own risk.

This code was written by GitHub user: budgettsfrog
Contact: budgettsfrog@protonmail.com
GitHub: https://github.com/warrenwoolseyiii*/

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
#include <version.h>

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
#define MAX_PAYLOAD_LEN 256

// Buffer length parameters
#define NUM_OVER_HEAD_BYTES 10
#define RX_BUFFER_LEN ((MAX_PAYLOAD_LEN + NUM_OVER_HEAD_BYTES) * 2)

// Error codes
#define SP_OK 0
#define SP_ERR_ILLEGAL_MSG_LEN -1
#define SP_ERR_NULL_PAYLOAD -2

    /**
     * @brief Set the my addr object, which is the address of the device that is running this code.
     * Messages that are receieved that do not match this address, or the broadcast address, are ignored.
     *
     * @param addr
     */
    void set_my_addr(uint8_t addr);

    /**
     * @brief Set the broadcast addr, which is the address that is used to broadcast messages to all devices.
     *
     * @param addr
     */
    void set_broadcast_addr(uint8_t addr);

    /**
     * @brief Parse the input buffer, and call the user_rcv_message function when a message is parsed and the address matches
     * the device address or the broadcast address.
     *
     * @param input_buffer - the buffer to parse
     * @param max_length - the maximum number of bytes to parse in one call, could be the length of the buffer or shorter.
     */
    void parse_input_buffer(uint8_t *input_buffer, uint32_t max_length);

    /**
     * @brief Send a message to the specified address. The message is sent by calling the user_serial_tx function.
     *
     * @param dest_addr - the address to send the message to.
     * @param type - the message type.
     * @param payload - the payload to send.
     * @param payload_length - the length of the payload in bytes.
     * @return int - the error code to return, SP_OK if no error.
     */
    int send_message(uint8_t dest_addr, uint8_t type, uint8_t *payload, uint32_t payload_length);

    /**
     * @brief Get the version of the serial protocol library.
     *
     * @return const char* - the version string.
     */
    char *get_version();

#ifdef __cplusplus
}
#endif

#endif /* SERIAL_PROTOCOL_H */
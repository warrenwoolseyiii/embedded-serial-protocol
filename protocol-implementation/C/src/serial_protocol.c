
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

#include <serial_protocol.h>
#include <user_impl.h>
#include <string.h>

uint32_t _parsing_index = 0;
uint32_t _current_message_len = 0;
uint8_t _my_addr = 0;
uint8_t _broadcast_addr = 0;
uint8_t _current_message[RX_BUFFER_LEN];

void _queue_byte(uint8_t byte)
{
    _current_message[_parsing_index] = byte;
    _parsing_index = (_parsing_index + 1) % RX_BUFFER_LEN;
}

void _reset_parsing_state()
{
    _parsing_index = 0;
    _current_message_len = 0;
    memset(_current_message, 0, RX_BUFFER_LEN);
}

uint16_t _crc16(uint8_t *data, uint32_t length)
{
    uint16_t crc = 0xFFFF;
    for (uint32_t i = 0; i < length; i++)
    {
        crc ^= data[i];
        for (uint32_t j = 0; j < 8; j++)
        {
            if (crc & 0x0001)
                crc = (crc >> 1) ^ 0xA001;
            else
                crc = crc >> 1;
        }
    }
    return crc;
}

void _parse_input(uint8_t input)
{
    // Parse the message if we cleared the header check
    if (_parsing_index > PAYLOAD_LEN_LSB_POS)
    {
        // Queue the byte
        _queue_byte(input);

        // Check if we have a complete message
        uint32_t total_len = _current_message_len + NUM_OVER_HEAD_BYTES;
        if (_parsing_index >= total_len)
        {
            // Calculate the CRC index to prevent an underflow
            uint32_t crc_index = _current_message_len + PAYLOAD_START_POS;
            if (crc_index >= RX_BUFFER_LEN)
                _reset_parsing_state();
            else
            {
                // Check the CRC
                uint16_t crc = _current_message[crc_index] << 8 | _current_message[crc_index + 1];
                uint16_t calculated_crc = _crc16(_current_message, (crc_index));

                // If the CRC is valid, and the message is meant for us, recieve the message!
                uint8_t tgt_addr = _current_message[TGT_ADDR_POS];
                if (crc == calculated_crc && (tgt_addr == _my_addr || tgt_addr == _broadcast_addr))
                {
                    user_rcv_message(_current_message, total_len);
                    _reset_parsing_state();
                }
                else
                    _reset_parsing_state();
            }
        }
    }
    else
    {
        switch (_parsing_index)
        {
        case HEADER_0_POS:
            if (input != HEADER_0_VAL)
                _reset_parsing_state();
            else
                _queue_byte(input);
            break;
        case HEADER_1_POS:
            if (input != HEADER_1_VAL)
                _reset_parsing_state();
            else
                _queue_byte(input);
            break;
        case HEADER_2_POS:
            if (input != HEADER_2_VAL)
                _reset_parsing_state();
            else
                _queue_byte(input);
            break;
        case SRC_ADDR_POS:
        case TGT_ADDR_POS:
        case MSG_TYPE_POS:
        case PAYLOAD_LEN_MSB_POS:
            _queue_byte(input);
            break;
        case PAYLOAD_LEN_LSB_POS:
            _queue_byte(input);

            // Set the current message length, protect against an oversized message
            _current_message_len = (uint32_t)(_current_message[PAYLOAD_LEN_MSB_POS] << 8) | _current_message[PAYLOAD_LEN_LSB_POS];
            if (_current_message_len > MAX_PAYLOAD_LEN)
                _reset_parsing_state();
            break;
        default:
            _reset_parsing_state();
            break;
        }
    }
}

void set_my_addr(uint8_t addr)
{
    _my_addr = addr;
}

void set_broadcast_addr(uint8_t addr)
{
    _broadcast_addr = addr;
}

void parse_input_buffer(uint8_t *input_buffer, uint32_t max_length)
{
    for (uint32_t i = 0; i < max_length; i++)
        _parse_input(input_buffer[i]);
}

int send_message(uint8_t dest_addr, uint8_t type, uint8_t *payload, uint32_t payload_length)
{
    uint8_t tx_buff[MAX_PAYLOAD_LEN + NUM_OVER_HEAD_BYTES];

    // Protect against an oversized message
    if (payload_length > MAX_PAYLOAD_LEN)
        return SP_ERR_ILLEGAL_MSG_LEN;

    // Set the header bytes
    tx_buff[HEADER_0_POS] = HEADER_0_VAL;
    tx_buff[HEADER_1_POS] = HEADER_1_VAL;
    tx_buff[HEADER_2_POS] = HEADER_2_VAL;
    tx_buff[SRC_ADDR_POS] = _my_addr;
    tx_buff[TGT_ADDR_POS] = dest_addr;
    tx_buff[MSG_TYPE_POS] = type;
    tx_buff[PAYLOAD_LEN_MSB_POS] = (uint8_t)((payload_length >> 8) & 0xFF);
    tx_buff[PAYLOAD_LEN_LSB_POS] = (uint8_t)(payload_length & 0xFF);

    // Copy the payload in
    if (payload && payload_length > 0)
        memcpy(tx_buff + PAYLOAD_START_POS, payload, payload_length);
    else if (payload_length > 0 && !payload)
        return SP_ERR_NULL_PAYLOAD;

    // Calculate the CRC
    uint16_t crc = _crc16(tx_buff, payload_length + PAYLOAD_START_POS);
    tx_buff[payload_length + PAYLOAD_START_POS] = (uint8_t)((crc >> 8) & 0xFF);
    tx_buff[payload_length + PAYLOAD_START_POS + 1] = (uint8_t)(crc & 0xFF);

    // Send the message!
    user_serial_tx(tx_buff, payload_length + NUM_OVER_HEAD_BYTES);
    return SP_OK;
}

char *get_version()
{
    int major = VERSION_MAJOR;
    int minor = VERSION_MINOR;
    int rev = VERSION_REV;

    // Place into a string, do not use dynamic memory allocation, don't use sprintf because it's not available on all platforms and we want to keep size to a minumum.
    static char version[32] = {0};
    version[0] = 'v';
    version[1] = (char)((major / 100) + '0');
    version[2] = (char)(((major % 100) / 10) + '0');
    version[3] = (char)((major % 10) + '0');
    version[4] = '.';
    version[5] = (char)((minor / 100) + '0');
    version[6] = (char)(((minor % 100) / 10) + '0');
    version[7] = (char)((minor % 10) + '0');
    version[8] = '.';
    version[9] = (char)((rev / 100) + '0');
    version[10] = (char)(((rev % 100) / 10) + '0');
    version[11] = (char)((rev % 10) + '0');
    return version;
}
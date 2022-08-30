/******** START PROTOCOL IMPLEMENTATION ********/
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
#include <string.h>

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
#define MAX_PAYLOAD_LEN 128

// Buffer length parameters
#define NUM_OVER_HEAD_BYTES 10
#define RX_BUFFER_LEN ((MAX_PAYLOAD_LEN + NUM_OVER_HEAD_BYTES) * 2)

// Device address - you need to override this in your implementation.
#define MY_ADDR 0x1A

// If this device is the host on the bus this is where you can implement periphal functionality.
#define HOST_ADDR 0x01
//#define PERIPHERAL_ADDR_1 0x02

// Protocol version
#define PROTOCOL_VERSION 0x0001

// Error codes
#define SP_OK 0
#define SP_ERR_ILLEGAL_MSG_LEN -1
#define SP_ERR_NULL_PAYLOAD -2

uint32_t _parsing_index = 0;
uint32_t _current_message_len = 0;
uint8_t _current_message[RX_BUFFER_LEN];

// User functions defined below
void user_rcv_message(uint8_t *msg_buf, uint32_t msg_len);
void user_serial_tx(uint8_t *msg_buf, uint32_t msg_len);

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
                if (crc == calculated_crc && tgt_addr == MY_ADDR)
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
    tx_buff[SRC_ADDR_POS] = MY_ADDR;
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
/********** END PROTOCOL IMPLEMENTATION **********/

// Enumerate all incoming message types from the host
typedef enum
{
    msg_version_req = 0,
    msg_ping_req = 1,
    msg_set_value_0 = 2,
    msg_get_value_0 = 3,
    msg_poll_for_data_req = 4,
} host_msg_type_t;

// Enumerate all outgoing message types to the host
typedef enum
{
    msg_version_resp = 0,
    msg_ping_resp = 1,
    msg_set_value_0_resp = 2,
    msg_get_value_0_resp = 3,
    msg_async_event_list_resp = 4,
    msg_nack = 5,
} device_msg_type_t;

// Global values for the device
uint8_t value_0 = 0;

// Asynchronous events
uint8_t async_event_1_pending = 0;
uint8_t async_event_2_pending = 0;
uint8_t async_event_3_pending = 0;
uint8_t alarm_1_pending = 0;

void user_rcv_message(uint8_t *msg_buf, uint32_t msg_len)
{
    // Check if the message is from the host, if not then nack it
    if (msg_buf[SRC_ADDR_POS] != HOST_ADDR)
    {
        // TODO: NACK the message
    }
    else
    {
        switch (msg_buf[MSG_TYPE_POS])
        {
        case msg_version_req:
        {
            uint8_t payload[] = {PROTOCOL_VERSION >> 8, PROTOCOL_VERSION & 0xFF};
            send_message(HOST_ADDR, msg_version_resp, payload, sizeof(payload));
        }
        break;
        case msg_ping_req:
            send_message(HOST_ADDR, msg_ping_resp, NULL, 0);
            break;
        case msg_set_value_0:
            value_0 = msg_buf[PAYLOAD_START_POS];
            send_message(HOST_ADDR, msg_set_value_0_resp, NULL, 0);
            break;
        case msg_get_value_0:
            send_message(HOST_ADDR, msg_get_value_0_resp, &value_0, sizeof(value_0));
            break;
        case msg_poll_for_data_req:
        {
            if (async_event_1_pending || async_event_2_pending || async_event_3_pending || alarm_1_pending)
            {
                uint8_t payload[] = {async_event_1_pending, async_event_2_pending, async_event_3_pending, alarm_1_pending};
                send_message(HOST_ADDR, msg_async_event_list_resp, payload, sizeof(payload));
                async_event_1_pending = 0;
                async_event_2_pending = 0;
                async_event_3_pending = 0;
                alarm_1_pending = 0;
            }
            else
            {
                send_message(HOST_ADDR, msg_nack, NULL, 0);
            }
        }
        break;
        default:
            send_message(HOST_ADDR, msg_nack, NULL, 0);
        }
    }
}

void user_serial_tx(uint8_t *msg_buf, uint32_t msg_len)
{
    Serial.write(msg_buf, msg_len);
}

void setup()
{
    Serial.begin(9600);
}

void loop()
{
    int avail = Serial.available();
    if (avail)
    {
        uint8_t input_buffer[16];
        Serial.readBytes(input_buffer, (avail > 16 ? 16 : avail));
        parse_input_buffer(input_buffer, avail);
    }
}
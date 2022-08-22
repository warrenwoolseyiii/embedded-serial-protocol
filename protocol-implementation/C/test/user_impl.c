#include "../src/user_impl.h"
#include "../src/serial_protocol.h"

// RX Types
#define RX_TYPE_0 0
#define RX_TYPE_1 1
#define RX_TYPE_2 2

typedef struct {
    uint8_t type;
    uint16_t length;
} message_descriptor_t;

message_descriptor_t rx_type_0 = {RX_TYPE_0, 0};
message_descriptor_t rx_type_1 = {RX_TYPE_1, MAX_PAYLOAD_LEN};
message_descriptor_t rx_type_2 = {RX_TYPE_2, 8};

void user_rcv_message( uint8_t *msg_buf, uint32_t msg_len )
{}

void user_serial_tx( uint8_t *msg_buf, uint32_t msg_len )
{}
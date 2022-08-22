#include "../src/user_impl.h"
#include "../src/serial_protocol.h"
#include "unit_test_glue.h"
#include <string.h>
#include <printf.h>

// RX Types
#define RX_TYPE_0 0
#define RX_TYPE_1 1
#define RX_TYPE_2 2

typedef struct
{
    uint8_t type;
    uint16_t length;
} message_descriptor_t;

message_descriptor_t rx_type_0 = {RX_TYPE_0, 0};
message_descriptor_t rx_type_1 = {RX_TYPE_1, MAX_PAYLOAD_LEN};
message_descriptor_t rx_type_2 = {RX_TYPE_2, 8};

void user_rcv_message(uint8_t *msg_buf, uint32_t msg_len)
{
}

// Global buffer and tx notifitcaion
uint8_t global_tx_buff[RX_BUFFER_LEN];
uint8_t global_tx_notify = 0;
void user_serial_tx(uint8_t *msg_buf, uint32_t msg_len)
{
    // Copy the message into the global notify
    memset(global_tx_buff, 0, RX_BUFFER_LEN);
    memcpy(global_tx_buff, msg_buf, msg_len);
    global_tx_notify = 1;

    // Print the message
    printf("user_serial_tx sending:");
    for (uint32_t i = 0; i < msg_len; i++)
        printf("%X", msg_buf[i]);
    printf("\n");
}

uint8_t get_user_tx_notify()
{
    return global_tx_notify;
}

void clear_user_tx_notify()
{
    global_tx_notify = 0;
}

uint8_t *get_user_tx_buff_ptr()
{
    return global_tx_buff;
}
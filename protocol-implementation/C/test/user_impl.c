
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

#include "../src/user_impl.h"
#include "../src/serial_protocol.h"
#include "unit_test_glue.h"
#include <string.h>
#include <stdio.h>

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

// Global buffer and rx notification
uint8_t global_rx_buf[RX_BUFFER_LEN];
uint8_t global_rx_notify = 0;
void user_rcv_message(uint8_t *msg_buf, uint32_t msg_len)
{
    memcpy(global_rx_buf, msg_buf, msg_len);
    global_rx_notify = 1;
}

uint8_t get_user_rx_notify()
{
    return global_rx_notify;
}

void clear_user_rx_notify()
{
    global_rx_notify = 0;
}

uint8_t *get_user_rx_buff_ptr()
{
    return global_rx_buf;
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
    // printf("user_serial_tx sending %d bytes:", msg_len);
    // for (uint32_t i = 0; i < msg_len; i++)
    //    printf("%X", msg_buf[i]);
    // printf("\n");
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
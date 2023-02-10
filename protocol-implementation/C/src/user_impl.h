#ifndef USER_IMPL_H
#define USER_IMPL_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

    /**
     * @brief User implementation of the receive message function. This function is called by the
     * serial parser when a message has been received. The entire buffer, including the
     * header and the checksum, is passed to the function.
     *
     * @param msg_buf - pointer to the message buffer, which contains the entire message, including the header and the checksum.
     * @param msg_len - length of the message, including the header and the checksum.
     */
    void user_rcv_message(uint8_t *msg_buf, uint32_t msg_len);

    /**
     * @brief User implementation of the transmit function. This function is called by send_message
     * to transmit the message on whatever physical hardware bus is being utilized. The entire message,
     * including the header and the checksum, is passed to the function.
     *
     * @param msg_buf - pointer to the message buffer, which contains the entire message, including the header and the checksum.
     * @param msg_len - length of the message, including the header and the checksum.
     */
    void user_serial_tx(uint8_t *msg_buf, uint32_t msg_len);

#ifdef __cplusplus
}
#endif

#endif /* USER_IMPL_H */
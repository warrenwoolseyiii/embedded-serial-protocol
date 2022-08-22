#ifndef USER_IMPL_H
#define USER_IMPL_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

    /*
     * User implementation of the receive message function. The serial parser
     * calls this function when a message is received. The entire buffer, including
     * the header and the checksum, is passed to the function.
     */
    void user_rcv_message(uint8_t *msg_buf, uint32_t msg_len);

    /*
     * User implementation of the send message function. This function is called by the
     * serial parser when a message needs to be sent. The entire buffer, including the
     * header and the checksum, is passed to the function.
     */
    void user_serial_tx(uint8_t *msg_buf, uint32_t msg_len);

#ifdef __cplusplus
}
#endif

#endif /* USER_IMPL_H */
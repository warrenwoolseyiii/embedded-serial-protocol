#ifndef USER_IMPL_H
#define USER_IMPL_H

#include <stdint.h>

/*
* User implementation of the receive message function. The serial parser
* calls this function when a message is received. The entire buffer, including 
* the header and the checksum, is passed to the function.
*/
void user_rcv_message( uint8_t *msg_buf, uint32_t msg_len );

/*
* User implementation of the send message function. This function is called by the
* serial parser when a message needs to be sent. The entire buffer, including the
* header and the checksum, is passed to the function.
*/
void user_serial_tx( uint8_t *msg_buf, uint32_t msg_len );

#endif /* USER_IMPL_H */

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

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

#ifndef UNIT_TEST_GLUE_H
#define UNIT_TEST_GLUE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

    uint8_t get_user_rx_notify();
    void clear_user_rx_notify();
    uint8_t *get_user_rx_buff_ptr();
    uint8_t get_user_tx_notify();
    void clear_user_tx_notify();
    uint8_t *get_user_tx_buff_ptr();

#ifdef __cplusplus
}
#endif

#endif /* UNIT_TEST_GLUE_H */
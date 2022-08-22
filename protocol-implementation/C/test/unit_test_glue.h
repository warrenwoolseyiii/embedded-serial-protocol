#ifndef UNIT_TEST_GLUE_H
#define UNIT_TEST_GLUE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

    uint8_t get_user_tx_notify();
    void clear_user_tx_notify();
    uint8_t *get_user_tx_buff_ptr();

#ifdef __cplusplus
}
#endif

#endif /* UNIT_TEST_GLUE_H */
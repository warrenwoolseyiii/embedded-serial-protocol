#include <gtest/gtest.h>
#include "../src/serial_protocol.h"
#include "../src/user_impl.h"

TEST(Message_Building, Valid_Header_No_Payload)
{
  uint8_t test_addr = 0;
  uint8_t test_type = 1;
  uint16_t test_payload_len = 0;
  int ret = send_message(test_addr, test_type, 0, test_payload_len);

  // Assert the message was queued successfuly
  ASSERT_EQ(ret, SP_OK);

  // Assert the tx callback was called
  extern uint8_t global_tx_notify;
  ASSERT_TRUE(global_tx_notify);

  // Assert the message was populated properly
  extern uint8_t global_tx_buff[RX_BUFFER_LEN];
  ASSERT_EQ(global_tx_buff[HEADER_0_POS], HEADER_0_VAL);
  ASSERT_EQ(global_tx_buff[HEADER_1_POS], HEADER_1_VAL);
  ASSERT_EQ(global_tx_buff[HEADER_2_POS], HEADER_2_VAL);
  ASSERT_EQ(global_tx_buff[SRC_ADDR_POS], MY_ADDR);
  ASSERT_EQ(global_tx_buff[TGT_ADDR_POS], test_addr);
  ASSERT_EQ(global_tx_buff[MSG_TYPE_POS], test_type);
  ASSERT_EQ(global_tx_buff[PAYLOAD_LEN_MSB_POS], (uint8_t)(test_payload_len << 8));
  ASSERT_EQ(global_tx_buff[PAYLOAD_LEN_LSB_POS], (uint8_t)(test_payload_len & 0xFF));
}
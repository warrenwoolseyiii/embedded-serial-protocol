#include <gtest/gtest.h>
#include "../src/serial_protocol.h"
#include "../src/user_impl.h"

// Demonstrate some basic assertions.
TEST(HelloTest, BasicAssertions)
{
  // Expect two strings not to be equal.
  EXPECT_STRNE("hello", "world");
  // Expect equality.
  EXPECT_EQ(7 * 6, 42);
}

TEST(Message_Building, Valid_Header_No_Payload)
{
  uint8_t test_addr = 0;
  uint8_t test_type = 1;
  uint16_t test_payload_len = 0;
  int ret = send_message(test_addr, test_type, 0, test_payload_len);

  // Assert the message was queued successfuly
  ASSERT_EQ(ret, SP_OK);
}
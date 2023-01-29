#include <gtest/gtest.h>
#include "../src/serial_protocol.h"
#include "unit_test_glue.h"
#include "../src/user_impl.h"

// This is a test address for the embedded protocol.
#define MY_ADDR 0xEE

class MessageBuilding : public ::testing::Test
{
public:
  MessageBuilding()
  {
    // initialization code here
  }

  void SetUp()
  {
    set_my_addr(MY_ADDR);
    clear_user_tx_notify();
  }

  void TearDown()
  {
    clear_user_tx_notify();
  }

  ~MessageBuilding()
  {
    // cleanup any pending stuff, but no exceptions allowed
  }

  // put in any custom data members that you need
};

TEST_F(MessageBuilding, Valid_Header_No_Payload)
{
  uint8_t test_addr = 0;
  uint8_t test_type = 1;
  uint16_t test_payload_len = 0;
  int ret = send_message(test_addr, test_type, 0, test_payload_len);

  // Assert the message was queued successfuly
  ASSERT_EQ(ret, SP_OK);

  // Assert the tx callback was called
  ASSERT_TRUE(get_user_tx_notify());

  // Assert the message was populated properly
  uint8_t *tx_ptr = get_user_tx_buff_ptr();
  ASSERT_EQ(tx_ptr[HEADER_0_POS], HEADER_0_VAL);
  ASSERT_EQ(tx_ptr[HEADER_1_POS], HEADER_1_VAL);
  ASSERT_EQ(tx_ptr[HEADER_2_POS], HEADER_2_VAL);
  ASSERT_EQ(tx_ptr[SRC_ADDR_POS], MY_ADDR);
  ASSERT_EQ(tx_ptr[TGT_ADDR_POS], test_addr);
  ASSERT_EQ(tx_ptr[MSG_TYPE_POS], test_type);
  ASSERT_EQ(tx_ptr[PAYLOAD_LEN_MSB_POS], (uint8_t)(test_payload_len >> 8));
  ASSERT_EQ(tx_ptr[PAYLOAD_LEN_LSB_POS], (uint8_t)(test_payload_len & 0xFF));
}

TEST_F(MessageBuilding, Valid_Header_Max_Payload)
{
  uint8_t test_addr = 0;
  uint8_t test_type = 1;
  uint16_t test_payload_len = MAX_PAYLOAD_LEN;
  uint8_t test_payload[MAX_PAYLOAD_LEN];
  memset(test_payload, 0xDD, MAX_PAYLOAD_LEN);
  int ret = send_message(test_addr, test_type, test_payload, test_payload_len);

  // Assert the message was queued successfuly
  ASSERT_EQ(ret, SP_OK);

  // Assert the tx callback was called
  ASSERT_TRUE(get_user_tx_notify());

  // Assert the message was populated properly
  uint8_t *tx_ptr = get_user_tx_buff_ptr();
  ASSERT_EQ(tx_ptr[HEADER_0_POS], HEADER_0_VAL);
  ASSERT_EQ(tx_ptr[HEADER_1_POS], HEADER_1_VAL);
  ASSERT_EQ(tx_ptr[HEADER_2_POS], HEADER_2_VAL);
  ASSERT_EQ(tx_ptr[SRC_ADDR_POS], MY_ADDR);
  ASSERT_EQ(tx_ptr[TGT_ADDR_POS], test_addr);
  ASSERT_EQ(tx_ptr[MSG_TYPE_POS], test_type);
  ASSERT_EQ(tx_ptr[PAYLOAD_LEN_MSB_POS], (uint8_t)(test_payload_len >> 8));
  ASSERT_EQ(tx_ptr[PAYLOAD_LEN_LSB_POS], (uint8_t)(test_payload_len & 0xFF));
  for (int i = 0; i < test_payload_len; i++)
    ASSERT_EQ(tx_ptr[i + PAYLOAD_START_POS], test_payload[i]);
}

TEST_F(MessageBuilding, Valid_Header_Null_Payload)
{
  uint8_t test_addr = 0;
  uint8_t test_type = 1;
  uint16_t test_payload_len = 1;
  uint8_t *test_payload = NULL;
  int ret = send_message(test_addr, test_type, test_payload, test_payload_len);

  // Assert the message was rejected
  ASSERT_EQ(ret, SP_ERR_NULL_PAYLOAD);
  ASSERT_FALSE(get_user_tx_notify());
}

TEST_F(MessageBuilding, Valid_Header_Too_Large_Payload)
{
  if (MAX_PAYLOAD_LEN == 65535)
  {
    printf("MAX_PAYLOAD_LEN is 65535, this test will always pass. Redefine MAX_PAYLOAD_LEN to be less than uint16_t max.\n");
  }
  else if (MAX_PAYLOAD_LEN > 65535)
  {
    printf("MAX_PAYLOAD_LEN is greater than 65535, that is an illegal value since the size parameter is uint16_t.\n");
    ASSERT_TRUE(false);
  }
  else
  {
    uint8_t test_addr = 0;
    uint8_t test_type = 1;
    uint16_t test_payload_len = MAX_PAYLOAD_LEN + 1;
    uint8_t test_payload[MAX_PAYLOAD_LEN];
    memset(test_payload, 0xAA, MAX_PAYLOAD_LEN);
    int ret = send_message(test_addr, test_type, test_payload, test_payload_len);

    // Assert the message was rejected
    ASSERT_EQ(ret, SP_ERR_ILLEGAL_MSG_LEN);
    ASSERT_FALSE(get_user_tx_notify());
  }
}

class MessageParsing : public ::testing::Test
{
public:
  MessageParsing()
  {
    // initialization code here
  }
  void SetUp()
  {
    clear_user_rx_notify();
  }
  void TearDown()
  {
    clear_user_rx_notify();
  }
  uint32_t MakeRandomMessage(int optional_len = -1)
  {
    uint8_t test_addr = MY_ADDR;
    uint8_t test_type = (rand() % 255) + 1;
    uint16_t test_payload_len = (rand() % MAX_PAYLOAD_LEN);

    // If we have a length set it here
    if (optional_len != -1)
      test_payload_len = (uint16_t)(optional_len & 0xFFFF);

    // make the payload
    uint8_t test_payload[MAX_PAYLOAD_LEN];
    for (int i = 0; i < test_payload_len; i++)
      test_payload[i] = (rand() % 255);

    // Send the message
    int ret = send_message(test_addr, test_type, test_payload, test_payload_len);

    // Return the result
    uint32_t total_len = (uint32_t)(NUM_OVER_HEAD_BYTES + test_payload_len);
    if (ret != SP_OK)
      total_len = 0;
    return total_len;
  }
  ~MessageParsing()
  {
    // cleanup any pending stuff, but no exceptions allowed
  }
  // put in any custom data members that you need
};

TEST_F(MessageParsing, Valid_Empty_Message)
{
  // Use the helper function to make a message with no payload, then get a pointer to the message
  uint32_t len = MessageParsing::MakeRandomMessage(0);
  uint8_t *message = get_user_tx_buff_ptr();

  // Parse the message
  parse_input_buffer(message, len);
  ASSERT_TRUE(get_user_rx_notify());
  uint8_t *parsed = get_user_rx_buff_ptr();
  ASSERT_EQ(memcmp(message, parsed, len), 0);
}

TEST_F(MessageParsing, Valid_Max_Length_Message)
{
  // use the helper function to make a message of max payload length
  uint32_t len = MessageParsing::MakeRandomMessage(MAX_PAYLOAD_LEN);
  uint8_t *message = get_user_tx_buff_ptr();

  // Parse the message
  parse_input_buffer(message, len);
  ASSERT_TRUE(get_user_rx_notify());
  uint8_t *parsed = get_user_rx_buff_ptr();
  ASSERT_EQ(memcmp(message, parsed, len), 0);
}

TEST_F(MessageParsing, Valid_Random_Length_Message)
{
  // use the helper function to make a message of random length
  uint32_t len = MessageParsing::MakeRandomMessage();
  uint8_t *message = get_user_tx_buff_ptr();

  // Parse the message
  parse_input_buffer(message, len);
  ASSERT_TRUE(get_user_rx_notify());
  uint8_t *parsed = get_user_rx_buff_ptr();
  ASSERT_EQ(memcmp(message, parsed, len), 0);
}

TEST_F(MessageParsing, Invalid_Preamble_Byte0)
{
  // use the helper function to make a message with random length
  uint32_t len = MessageParsing::MakeRandomMessage();
  uint8_t *message = get_user_tx_buff_ptr();
  message[0] = 0xAB;

  // Parse the message
  parse_input_buffer(message, len);
  ASSERT_FALSE(get_user_rx_notify());
}

TEST_F(MessageParsing, Invalid_Preamble_Byte1)
{
  // use the helper function to make a message with random length
  uint32_t len = MessageParsing::MakeRandomMessage();
  uint8_t *message = get_user_tx_buff_ptr();
  message[1] = 0x56;

  // Parse the message
  parse_input_buffer(message, len);
  ASSERT_FALSE(get_user_rx_notify());
}

TEST_F(MessageParsing, Invalid_Preamble_Byte2)
{
  // use the helper function to make a message with random length
  uint32_t len = MessageParsing::MakeRandomMessage();
  uint8_t *message = get_user_tx_buff_ptr();
  message[2] = 0xFE;

  // Parse the message
  parse_input_buffer(message, len);
  ASSERT_FALSE(get_user_rx_notify());
}

TEST_F(MessageParsing, Invalid_Target_addr)
{
  // use the helper function to make a message with random length
  uint32_t len = MessageParsing::MakeRandomMessage();
  uint8_t *message = get_user_tx_buff_ptr();
  message[TGT_ADDR_POS] = MY_ADDR + 1;

  // Parse the message
  parse_input_buffer(message, len);
  ASSERT_FALSE(get_user_rx_notify());
}

TEST_F(MessageParsing, Garbage_Byte_To_Message)
{
  // use the helper function to make a message with random length
  uint32_t len = MessageParsing::MakeRandomMessage();
  uint8_t *message = get_user_tx_buff_ptr();

  // Parse the garbage byte
  uint8_t b = 0xFF;
  parse_input_buffer(&b, 1);
  ASSERT_FALSE(get_user_rx_notify());

  // Parse the message
  parse_input_buffer(message, len);
  ASSERT_TRUE(get_user_rx_notify());
  uint8_t *parsed = get_user_rx_buff_ptr();
  ASSERT_EQ(memcmp(message, parsed, len), 0);
}

TEST_F(MessageParsing, Bad_CRC)
{
  // use the helper function to make a message with random length
  uint32_t len = MessageParsing::MakeRandomMessage();
  uint8_t *message = get_user_tx_buff_ptr();
  message[len - 1] = 0xFF;

  // Parse the message
  parse_input_buffer(message, len);
  ASSERT_FALSE(get_user_rx_notify());
}

TEST_F(MessageParsing, Back_To_Back_Messages)
{
  // use the helper function to make a message with random length
  uint32_t len = MessageParsing::MakeRandomMessage();
  uint8_t *message = get_user_tx_buff_ptr();

  // Parse the message
  parse_input_buffer(message, len);
  ASSERT_TRUE(get_user_rx_notify());
  uint8_t *parsed = get_user_rx_buff_ptr();
  ASSERT_EQ(memcmp(message, parsed, len), 0);

  // Make another message
  uint32_t len2 = MessageParsing::MakeRandomMessage();

  // Parse the message
  parse_input_buffer(message, len2);
  ASSERT_TRUE(get_user_rx_notify());
  parsed = get_user_rx_buff_ptr();
  ASSERT_EQ(memcmp(message, parsed, len2), 0);
}

TEST_F(MessageParsing, Back_To_Back_Messages_Not_For_Me)
{
  // use the helper function to make a message with random length
  uint32_t len = MessageParsing::MakeRandomMessage();
  uint8_t *message = get_user_tx_buff_ptr();
  message[TGT_ADDR_POS] = MY_ADDR + 1;

  // Parse the message
  parse_input_buffer(message, len);
  ASSERT_FALSE(get_user_rx_notify());

  // Make another message
  uint32_t len2 = MessageParsing::MakeRandomMessage();

  // Parse the message
  parse_input_buffer(message, len2);
  ASSERT_TRUE(get_user_rx_notify());
  uint8_t *parsed = get_user_rx_buff_ptr();
  ASSERT_EQ(memcmp(message, parsed, len2), 0);
}
import unittest
import protocol as prot
from random import randint


# Helper function to generate a random message
def generate_random_message(length=-1, addr=prot.my_addr):
    # Make a random message
    m_type = randint(0, 255)
    t_addr = addr
    if length == -1:
        length = randint(0, prot.MAX_MSG_LEN)
    m_payload = [randint(0, 255) for i in range(length)]
    msg = prot.message(m_type, prot.my_addr, t_addr, len(m_payload), m_payload,
                       0)
    msg.msg_crc = prot.calculate_crc(msg)

    # Queue it into a list
    msg_list = []
    msg_list.append(prot.HEADER_BYTE0)
    msg_list.append(prot.HEADER_BYTE1)
    msg_list.append(prot.HEADER_BYTE2)
    msg_list.append(msg.src_addr)
    msg_list.append(msg.tgt_addr)
    msg_list.append(msg.msg_type)
    msg_list.append(msg.msg_len >> 8)
    msg_list.append(msg.msg_len & 0xFF)
    for i in range(msg.msg_len):
        msg_list.append(msg.msg_payload[i])
    msg_list.append(msg.msg_crc >> 8)
    msg_list.append(msg.msg_crc & 0xFF)
    return msg_list


class TestSender(unittest.TestCase):
    # Test the sender function with an empty payload
    def test_valid_msg_no_payload(self):
        m_type = 0x01
        t_addr = 0x02
        m_payload = []
        msg = prot.build_message(m_type, t_addr, m_payload)

        # Assert that the message is built properly
        self.assertEqual(m_payload, msg.msg_payload)
        self.assertEqual(m_type, msg.msg_type)
        self.assertEqual(prot.my_addr, msg.src_addr)
        self.assertEqual(t_addr, msg.tgt_addr)
        self.assertEqual(len(m_payload), msg.msg_len)

    # Test the sender function with a max payload length
    def test_valid_msg_max_payload(self):
        m_type = 0x01
        t_addr = 0x02
        m_payload = []
        for i in range(prot.MAX_MSG_LEN):
            m_payload.append(randint(0, 255))

        msg = prot.build_message(m_type, t_addr, m_payload)

        # Assert that the message is built properly
        self.assertEqual(m_payload, msg.msg_payload)
        self.assertEqual(m_type, msg.msg_type)
        self.assertEqual(prot.my_addr, msg.src_addr)
        self.assertEqual(t_addr, msg.tgt_addr)
        self.assertEqual(len(m_payload), msg.msg_len)

    # Test the sender function with a payload length greater than the max
    def test_invalid_msg_payload_too_long(self):
        m_type = 0x01
        t_addr = 0x02
        m_payload = []
        for i in range(prot.MAX_MSG_LEN + 1):
            m_payload.append(randint(0, 255))

        try:
            msg = prot.build_message(m_type, t_addr, m_payload)
            self.assertTrue(False)
        except Exception as e:
            self.assertTrue(True)

    # Test the sender function with a null payload list
    def test_invalid_msg_payload_null(self):
        m_type = 0x01
        t_addr = 0x02
        m_payload = None

        msg = prot.build_message(m_type, t_addr, m_payload)

        # Assert that the message is built properly
        self.assertEqual([], msg.msg_payload)
        self.assertEqual(m_type, msg.msg_type)
        self.assertEqual(prot.my_addr, msg.src_addr)
        self.assertEqual(t_addr, msg.tgt_addr)
        self.assertEqual(0, msg.msg_len)


class TestParser(unittest.TestCase):
    # Test setup method
    def setUp(self):
        _ = prot.check_for_parsed_messages()

    # Test teardown method
    def tearDown(self):
        _ = prot.check_for_parsed_messages()

    # Test the parser function with a valid message with no payload
    def test_valid_msg_no_payload(self):
        msg = generate_random_message(0)
        prot.parse_input_buffer(msg)

        # Get the parsed messages
        msg_list = prot.check_for_parsed_messages()

        # Assert that the message is parsed properly
        self.assertEqual(1, len(msg_list))
        self.assertEqual(msg[3], msg_list[0].src_addr)
        self.assertEqual(msg[4], msg_list[0].tgt_addr)
        self.assertEqual(msg[5], msg_list[0].msg_type)

        msg_len = msg[6] << 8 | msg[7]
        self.assertEqual(msg_len, msg_list[0].msg_len)

        for i in range(msg_len):
            self.assertEqual(msg[8 + i], msg_list[0].msg_payload[i])

    # Test the parser function with a valid message with a max payload length
    def test_valid_msg_max_payload(self):
        msg = generate_random_message(prot.MAX_MSG_LEN)
        prot.parse_input_buffer(msg)

        # Get the parsed messages
        msg_list = prot.check_for_parsed_messages()

        # Assert that the message is parsed properly
        self.assertEqual(1, len(msg_list))
        self.assertEqual(msg[3], msg_list[0].src_addr)
        self.assertEqual(msg[4], msg_list[0].tgt_addr)
        self.assertEqual(msg[5], msg_list[0].msg_type)

        msg_len = msg[6] << 8 | msg[7]
        self.assertEqual(msg_len, msg_list[0].msg_len)

        for i in range(msg_len):
            self.assertEqual(msg[8 + i], msg_list[0].msg_payload[i])

    # Test the parser function with a null byte list
    def test_invalid_msg_null_bytes(self):
        msg = None
        prot.parse_input_buffer(msg)

        # Get the parsed messages
        msg_list = prot.check_for_parsed_messages()

        # Assert that the message is parsed properly
        self.assertEqual(0, len(msg_list))

    # Test the parser function with a message with a payload length greater than the max
    def test_invalid_msg_payload_too_long(self):
        msg = generate_random_message(prot.MAX_MSG_LEN + 1)
        prot.parse_input_buffer(msg)

        # Get the parsed messages
        msg_list = prot.check_for_parsed_messages()

        # Assert that the message is parsed properly
        self.assertEqual(0, len(msg_list))

    # Test the parser function with an invalid header byte 0
    def test_invalid_msg_header_byte0(self):
        msg = generate_random_message(0)
        msg[0] = 0xAB
        prot.parse_input_buffer(msg)

        # Get the parsed messages
        msg_list = prot.check_for_parsed_messages()

        # Assert that the message is parsed properly
        self.assertEqual(0, len(msg_list))

    # Test the parser function with an invalid header byte 1
    def test_invalid_msg_header_byte1(self):
        msg = generate_random_message(0)
        msg[1] = 0x56
        prot.parse_input_buffer(msg)

        # Get the parsed messages
        msg_list = prot.check_for_parsed_messages()

        # Assert that the message is parsed properly
        self.assertEqual(0, len(msg_list))

    # Test the parser function with an invalid header byte 2
    def test_invalid_msg_header_byte2(self):
        msg = generate_random_message(0)
        msg[2] = 0xFE
        prot.parse_input_buffer(msg)

        # Get the parsed messages
        msg_list = prot.check_for_parsed_messages()

        # Assert that the message is parsed properly
        self.assertEqual(0, len(msg_list))

    # Test the parser function with a message that is not for me
    def test_invalid_msg_not_for_me(self):
        msg = generate_random_message(0,prot.my_addr + 1)
        prot.parse_input_buffer(msg)

        # Get the parsed messages
        msg_list = prot.check_for_parsed_messages()

        # Assert that the message is parsed properly
        self.assertEqual(0, len(msg_list))

    # Test back to back valid messages
    def test_valid_msg_back_to_back(self):
        msg = generate_random_message()
        prot.parse_input_buffer(msg)

        msg1 = generate_random_message()
        prot.parse_input_buffer(msg1)

        # Get the parsed messages
        msg_list = prot.check_for_parsed_messages()

        # Assert that the message is parsed properly
        self.assertEqual(2, len(msg_list))
        self.assertEqual(msg[3], msg_list[0].src_addr)
        self.assertEqual(msg[4], msg_list[0].tgt_addr)
        self.assertEqual(msg[5], msg_list[0].msg_type)

        msg_len = msg[6] << 8 | msg[7]
        self.assertEqual(msg_len, msg_list[0].msg_len)

        for i in range(msg_len):
            self.assertEqual(msg[8 + i], msg_list[0].msg_payload[i])

        self.assertEqual(msg1[3], msg_list[1].src_addr)
        self.assertEqual(msg1[4], msg_list[1].tgt_addr)
        self.assertEqual(msg1[5], msg_list[1].msg_type)

        msg_len = msg1[6] << 8 | msg1[7]
        self.assertEqual(msg_len, msg_list[1].msg_len)

        for i in range(msg_len):
            self.assertEqual(msg1[8 + i], msg_list[1].msg_payload[i])

    # Test the parser with back to back valid messages, one for me and one not
    def test_valid_msg_back_to_back_not_for_me(self):
        msg = generate_random_message(-1, prot.my_addr + 1)
        prot.parse_input_buffer(msg)

        msg1 = generate_random_message()
        prot.parse_input_buffer(msg1)

        # Get the parsed messages
        msg_list = prot.check_for_parsed_messages()

        # Assert that the message is parsed properly
        self.assertEqual(1, len(msg_list))
        self.assertEqual(msg1[3], msg_list[0].src_addr)
        self.assertEqual(msg1[4], msg_list[0].tgt_addr)
        self.assertEqual(msg1[5], msg_list[0].msg_type)

        msg_len = msg1[6] << 8 | msg1[7]
        self.assertEqual(msg_len, msg_list[0].msg_len)

        for i in range(msg_len):
            self.assertEqual(msg1[8 + i], msg_list[0].msg_payload[i])

    # Test the parser for a broadcasted message
    def test_valid_msg_broadcast(self):
        msg = generate_random_message(-1, prot.broadcast_addr)
        prot.parse_input_buffer(msg)

        # Get the parsed messages
        msg_list = prot.check_for_parsed_messages()

        # Assert that the message is parsed properly
        self.assertEqual(1, len(msg_list))
        self.assertEqual(msg[3], msg_list[0].src_addr)
        self.assertEqual(msg[4], msg_list[0].tgt_addr)
        self.assertEqual(msg[5], msg_list[0].msg_type)

        msg_len = msg[6] << 8 | msg[7]
        self.assertEqual(msg_len, msg_list[0].msg_len)

        for i in range(msg_len):
            self.assertEqual(msg[8 + i], msg_list[0].msg_payload[i])

if __name__ == '__main__':
    unittest.main()

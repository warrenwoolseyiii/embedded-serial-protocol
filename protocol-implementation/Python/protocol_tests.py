import unittest
import protocol as prot
import user_impl as user
from random import randint


# Helper function to generate a random message
def generate_random_message(length=-1):
    # Make a random message
    m_type = randint(0, 255)
    t_addr = prot.MY_ADDR
    if length == -1:
        length = randint(0, prot.MAX_MSG_LEN)
    m_payload = [randint(0, 255) for i in range(length)]
    msg = prot.message(m_type, prot.MY_ADDR, t_addr, len(m_payload), m_payload,
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
    # Test setup method
    def setUp(self):
        user.user_set_tx_notify(False)

    # Test teardown method
    def tearDown(self):
        user.user_set_tx_notify(False)

    # Test the sender function with an empty payload
    def test_valid_msg_no_payload(self):
        m_type = 0x01
        t_addr = 0x02
        m_payload = []
        self.assertEqual(prot.send_message(m_type, t_addr, m_payload),
                         prot.error_state.NO_ERROR)
        self.assertTrue(user.user_get_tx_notify())

    # Test the sender function with a max payload length
    def test_valid_msg_max_payload(self):
        m_type = 0x01
        t_addr = 0x02
        m_payload = []
        for i in range(prot.MAX_MSG_LEN):
            m_payload.append(randint(0, 255))

        self.assertEqual(prot.send_message(m_type, t_addr, m_payload),
                         prot.error_state.NO_ERROR)
        self.assertTrue(user.user_get_tx_notify())

    # Test the sender function with a payload length greater than the max
    def test_invalid_msg_payload_too_long(self):
        m_type = 0x01
        t_addr = 0x02
        m_payload = []
        for i in range(prot.MAX_MSG_LEN + 1):
            m_payload.append(randint(0, 255))

        self.assertEqual(prot.send_message(m_type, t_addr, m_payload),
                         prot.error_state.ILLEGAL_MESSAGE_LENGTH)
        self.assertFalse(user.user_get_tx_notify())

    # Test the sender function with a null payload list
    def test_invalid_msg_payload_null(self):
        m_type = 0x01
        t_addr = 0x02
        m_payload = None
        self.assertEqual(prot.send_message(m_type, t_addr, m_payload),
                         prot.error_state.NO_ERROR)
        self.assertTrue(user.user_get_tx_notify())


class TestParser(unittest.TestCase):
    # Test setup method
    def setUp(self):
        user.user_set_rx_notify(False)

    # Test teardown method
    def tearDown(self):
        user.user_set_rx_notify(False)

    # Test the parser function with a valid message with no payload
    def test_valid_msg_no_payload(self):
        msg = generate_random_message(0)
        prot.parse_input_buffer(msg)
        self.assertTrue(user.user_get_rx_notify())

    # Test the parser function with a valid message with a max payload length
    def test_valid_msg_max_payload(self):
        msg = generate_random_message(prot.MAX_MSG_LEN)
        prot.parse_input_buffer(msg)
        self.assertTrue(user.user_get_rx_notify())

    # Test the parser function with a null byte list
    def test_invalid_msg_null_bytes(self):
        msg = None
        prot.parse_input_buffer(msg)
        self.assertFalse(user.user_get_rx_notify())

    # Test the parser function with a message with a payload length greater than the max
    def test_invalid_msg_payload_too_long(self):
        msg = generate_random_message(prot.MAX_MSG_LEN + 1)
        prot.parse_input_buffer(msg)
        self.assertFalse(user.user_get_rx_notify())

    # Test the parser function with an invalid header byte 0
    def test_invalid_msg_header_byte0(self):
        msg = generate_random_message(0)
        msg[0] = 0xAB
        prot.parse_input_buffer(msg)
        self.assertFalse(user.user_get_rx_notify())

    # Test the parser function with an invalid header byte 1
    def test_invalid_msg_header_byte1(self):
        msg = generate_random_message(0)
        msg[1] = 0x56
        prot.parse_input_buffer(msg)
        self.assertFalse(user.user_get_rx_notify())

    # Test the parser function with an invalid header byte 2
    def test_invalid_msg_header_byte2(self):
        msg = generate_random_message(0)
        msg[2] = 0xFE
        prot.parse_input_buffer(msg)
        self.assertFalse(user.user_get_rx_notify())

    # Test the parser function with a message that is not for me
    def test_invalid_msg_not_for_me(self):
        msg = generate_random_message(0)
        msg[4] = prot.MY_ADDR + 1
        prot.parse_input_buffer(msg)
        self.assertFalse(user.user_get_rx_notify())

    # Test back to back valid messages
    def test_valid_msg_back_to_back(self):
        msg = generate_random_message()
        prot.parse_input_buffer(msg)
        self.assertTrue(user.user_get_rx_notify())

        user.user_set_rx_notify(False)
        msg = generate_random_message()
        prot.parse_input_buffer(msg)
        self.assertTrue(user.user_get_rx_notify())

    # Test the parser with back to back valid messages, one for me and one not
    def test_valid_msg_back_to_back_not_for_me(self):
        msg = generate_random_message()
        msg[4] = prot.MY_ADDR + 1
        prot.parse_input_buffer(msg)
        self.assertFalse(user.user_get_rx_notify())

        msg = generate_random_message()
        prot.parse_input_buffer(msg)
        self.assertTrue(user.user_get_rx_notify())


if __name__ == '__main__':
    unittest.main()
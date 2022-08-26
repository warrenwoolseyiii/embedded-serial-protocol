import unittest
import protocol as prot
from random import randint


class TestSender(unittest.TestCase):
    # Test the sender function with an empty payload
    def test_valid_msg_no_payload(self):
        m_type = 0x01
        t_addr = 0x02
        m_payload = []
        self.assertEqual(prot.send_message(m_type, t_addr, m_payload),
                         prot.error_state.NO_ERROR)

    # Test the sender function with a max payload length
    def test_valid_msg_max_payload(self):
        m_type = 0x01
        t_addr = 0x02
        m_payload = []
        for i in range(prot.MAX_MSG_LEN):
            m_payload.append(randint(0, 255))

        self.assertEqual(prot.send_message(m_type, t_addr, m_payload),
                         prot.error_state.NO_ERROR)

    # Test the sender function with a payload length greater than the max
    def test_invalid_msg_payload_too_long(self):
        m_type = 0x01
        t_addr = 0x02
        m_payload = []
        for i in range(prot.MAX_MSG_LEN + 1):
            m_payload.append(randint(0, 255))

        self.assertEqual(prot.send_message(m_type, t_addr, m_payload),
                         prot.error_state.ILLEGAL_MESSAGE_LENGTH)

    # Test the sender function with a null payload list
    def test_invalid_msg_payload_null(self):
        m_type = 0x01
        t_addr = 0x02
        m_payload = None
        self.assertEqual(prot.send_message(m_type, t_addr, m_payload),
                         prot.error_state.NO_ERROR)

if __name__ == '__main__':
    unittest.main()
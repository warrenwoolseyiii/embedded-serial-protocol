from enum import Enum


# Enumeration of errors in the state machine
class error_state(Enum):
    NO_ERROR = 0
    ILLEGAL_MESSAGE_LENGTH = 1


# Enumeration of the states in the state machine
class parsing_state(Enum):
    HEADER_POS0 = 0
    HEADER_POS1 = 1
    HEADER_POS2 = 2
    SRC_ADDR_POS = 3
    TGT_ADDR_POS = 4
    MSG_TYPE_POS = 5
    PAYLOAD_LEN_MSB_POS = 6
    PAYLOAD_LEN_LSB_POS = 7
    PAYLOAD_START_POS = 8
    CRC_POS_1 = 9
    CRC_POS_2 = 10


# Constant values for the protocol
HEADER_BYTE0 = 0xAA
HEADER_BYTE1 = 0x55
HEADER_BYTE2 = 0xFF

# User address, you can change this to whatever you want
my_addr = 0x01
broadcast_addr = 0xFF

# Maximum allowable length, you can change this - there is a maximum value of 65535 bytes
MAX_MSG_LEN = 65535


class message:
    error = error_state.NO_ERROR
    msg_type = 0
    src_addr = 0
    tgt_addr = 0
    msg_len = 0
    msg_payload = []
    msg_crc = 0

    def __init__(self, msg_type, src_addr, tgt_addr, msg_len, msg_payload,
                 msg_crc):
        self.error = error_state.NO_ERROR
        self.msg_type = msg_type
        self.src_addr = src_addr
        self.tgt_addr = tgt_addr
        self.msg_len = msg_len
        self.msg_payload = msg_payload
        self.msg_crc = msg_crc

    def __str__(self):
        return "msg_type: {}, src_addr: {}, tgt_addr: {}, msg_len: {}, msg_payload: {}, msg_crc: {}, error: {}".format(
            hex(self.msg_type), hex(self.src_addr), hex(self.tgt_addr),
            hex(self.msg_len), self.msg_payload, hex(self.msg_crc), self.error)

    def to_list(self):
        msg_list = []
        msg_list.append(HEADER_BYTE0)
        msg_list.append(HEADER_BYTE1)
        msg_list.append(HEADER_BYTE2)
        msg_list.append(self.src_addr)
        msg_list.append(self.tgt_addr)
        msg_list.append(self.msg_type)
        msg_list.append(self.msg_len >> 8 & 0xFF)
        msg_list.append(self.msg_len & 0xFF)
        for i in range(self.msg_len):
            msg_list.append(self.msg_payload[i])
        msg_list.append(self.msg_crc >> 8 & 0xFF)
        msg_list.append(self.msg_crc & 0xFF)
        return msg_list


# Parsing state global variable
p_state = parsing_state(0)

# Current message global variable
current_msg = message(0, 0, 0, 0, [], 0)

# Global parsed notification and storage
parsed_message_queue = []
message_available = False


# Set my address
def set_my_address(addr):
    global my_addr
    my_addr = addr


# Internal notify of a parsed message
def notify_parsed_message(msg):
    global parsed_message_queue
    global message_available
    parsed_message_queue.append(msg)
    message_available = True


# Calculate the CRC of the message
def calculate_crc(msg):
    crc = 0xFFFF
    byte_list = []
    byte_list.append(HEADER_BYTE0)
    byte_list.append(HEADER_BYTE1)
    byte_list.append(HEADER_BYTE2)
    byte_list.append(msg.src_addr)
    byte_list.append(msg.tgt_addr)
    byte_list.append(msg.msg_type)
    byte_list.append(msg.msg_len >> 8 & 0xFF)
    byte_list.append(msg.msg_len & 0xFF)
    for i in range(msg.msg_len):
        byte_list.append(msg.msg_payload[i])
    for i in range(len(byte_list)):
        crc = crc ^ byte_list[i]
        for j in range(8):
            if (crc & 0x0001):
                crc = crc >> 1
                crc = crc ^ 0xA001
            else:
                crc = crc >> 1
    return crc


# Reset the parsing state machine
def reset_parsing_state():
    global p_state
    global current_msg
    p_state = parsing_state.HEADER_POS0
    current_msg = message(0, 0, 0, 0, [], 0)


# Parse the incoming byte in the state machine
def parse_byte(byte):
    global p_state
    global current_msg
    # Switch based on the state
    if p_state == parsing_state.HEADER_POS0:
        if byte == HEADER_BYTE0:
            p_state = parsing_state.HEADER_POS1
        else:
            reset_parsing_state()
    elif p_state == parsing_state.HEADER_POS1:
        if byte == HEADER_BYTE1:
            p_state = parsing_state.HEADER_POS2
        else:
            reset_parsing_state()
    elif p_state == parsing_state.HEADER_POS2:
        if byte == HEADER_BYTE2:
            p_state = parsing_state.SRC_ADDR_POS
        else:
            reset_parsing_state()
    elif p_state == parsing_state.SRC_ADDR_POS:
        current_msg.src_addr = byte
        p_state = parsing_state.TGT_ADDR_POS
    elif p_state == parsing_state.TGT_ADDR_POS:
        current_msg.tgt_addr = byte
        p_state = parsing_state.MSG_TYPE_POS
    elif p_state == parsing_state.MSG_TYPE_POS:
        current_msg.msg_type = byte
        p_state = parsing_state.PAYLOAD_LEN_MSB_POS
    elif p_state == parsing_state.PAYLOAD_LEN_MSB_POS:
        current_msg.msg_len = byte << 8
        p_state = parsing_state.PAYLOAD_LEN_LSB_POS
    elif p_state == parsing_state.PAYLOAD_LEN_LSB_POS:
        current_msg.msg_len |= byte
        if current_msg.msg_len > MAX_MSG_LEN:
            reset_parsing_state()
            raise Exception("Message length too large")
        elif current_msg.msg_len > 0:
            p_state = parsing_state.PAYLOAD_START_POS
        else:
            p_state = parsing_state.CRC_POS_1
    elif p_state == parsing_state.PAYLOAD_START_POS:
        current_msg.msg_payload.append(byte)
        if len(current_msg.msg_payload) == current_msg.msg_len:
            p_state = parsing_state.CRC_POS_1
    elif p_state == parsing_state.CRC_POS_1:
        current_msg.msg_crc = (byte & 0xFF) << 8
        p_state = parsing_state.CRC_POS_2
    elif p_state == parsing_state.CRC_POS_2:
        current_msg.msg_crc |= (byte & 0xFF)
        # Check the CRC
        global my_addr
        if current_msg.msg_crc == calculate_crc(
                current_msg) and (current_msg.tgt_addr == my_addr or current_msg.tgt_addr == broadcast_addr):
            # CRC is good, send the message to the message handler
            notify_parsed_message(current_msg)
            reset_parsing_state()
        else:
            # CRC is bad, reset the parsing state machine
            reset_parsing_state()


# Parse an incoming list of bytes into the state machine
def parse_input_buffer(input_buffer):
    try:
        # Parse the input buffer into the state machine
        for byte in input_buffer:
            parse_byte(byte)
    except Exception as e:
        print("Exception: " + str(e))


# Check for parsed messages in the queue
def check_for_parsed_messages():
    global parsed_message_queue
    global message_available
    if message_available:
        message_available = False
        queue = parsed_message_queue.copy()
        parsed_message_queue.clear()
        return queue
    else:
        return []


# Send a message to the message handler
def build_message(type, addr, payload):
    # Check for null payload
    if payload is None:
        payload = []

    # Make sure we don't have an illegal length
    if len(payload) > MAX_MSG_LEN:
        raise Exception("Message length too large")

    # Create the message
    global my_addr
    msg = message(type, my_addr, addr, len(payload), payload, 0)
    # Calculate the CRC
    msg.msg_crc = calculate_crc(msg)
    # Return the message
    msg.error = error_state.NO_ERROR
    return msg

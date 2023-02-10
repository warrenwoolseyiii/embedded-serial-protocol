
#DISCLAIMER:

#This code is protected under the MIT open source license. The code is provided
#"as is" without warranty of any kind, either express or implied, including but
#not limited to the implied warranties of merchantability, fitness for a particular
#purpose, or non-infringement. In no event shall the author or any other party be
#liable for any direct, indirect, incidental, special, exemplary, or consequential
#damages, however caused and on any theory of liability, whether in contract,
#strict liability, or tort (including negligence or otherwise), arising in any way
#out of the use of this code or performance or use of the results of this code. By
#using this code, you agree to hold the author and any other party harmless from
#any and all liability and to use the code at your own risk.

#This code was written by GitHub user: budgettsfrog
#Contact: budgettsfrog@protonmail.com
#GitHub: https://github.com/warrenwoolseyiii

from enum import Enum
from emb_ser_protocol import version as ver


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

# Maximum allowable length, you can change this - there is a maximum value of 65535 bytes
MAX_MSG_LEN = 65535


class message:
    """
    Class for a message

    Attributes:
        error: Error state of the message
        msg_type: Type of the message
        src_addr: Source address of the message (sender)
        tgt_addr: Target address of the message (receiver)
        msg_len: Length of the message payload in bytes
        msg_payload: Payload of the message
        msg_crc: CRC of the message
    """
    error = error_state.NO_ERROR
    msg_type = 0
    src_addr = 0
    tgt_addr = 0
    msg_len = 0
    msg_payload = []
    msg_crc = 0

    def __init__(self, msg_type, src_addr, tgt_addr, msg_len, msg_payload, msg_crc):
        """
        Constructor for the message class

        Args:
            msg_type: Type of the message
            src_addr: Source address of the message (sender)
            tgt_addr: Target address of the message (receiver)
            msg_len: Length of the message payload in bytes
            msg_payload: Payload of the message
            msg_crc: CRC of the message
        """
        self.error = error_state.NO_ERROR
        self.msg_type = msg_type
        self.src_addr = src_addr
        self.tgt_addr = tgt_addr
        self.msg_len = msg_len
        self.msg_payload = msg_payload
        self.msg_crc = msg_crc

    def __str__(self):
        """
        String representation of the message class

        Returns:
            String representation of the message class
        """
        return "msg_type: {}, src_addr: {}, tgt_addr: {}, msg_len: {}, msg_payload: {}, msg_crc: {}, error: {}".format(
            hex(self.msg_type), hex(self.src_addr), hex(self.tgt_addr),
            hex(self.msg_len), self.msg_payload, hex(self.msg_crc), self.error)

    def to_list(self):
        """
        Convert the message to a list of bytes

        Returns:
            List of bytes representing the message
        """
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
    """
    Set the address of the current device.

    Args:
        addr: Address of the current device
    """
    global my_addr
    my_addr = addr


# Internal notify of a parsed message
def notify_parsed_message(msg):
    """
    Notify the user that a message has been parsed, and store the message in the parsed_message_queue list.
    Sets the message_available global variable to True.

    Args:
        msg: Message that has been parsed
    """
    global parsed_message_queue
    global message_available
    parsed_message_queue.append(msg)
    message_available = True


# Calculate the CRC of the message
def calculate_crc(msg):
    """
    Calculate the CRC of the message.

    Args:
        msg: Message to calculate the CRC of
    """
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
    """
    Reset the parsing state machine. Sets the p_state global variable to HEADER_POS0. 
    Sets the current_msg global variable to a new message. If a message is successfully parsed, 
    this method will call notify_parsed_message.
    """
    global p_state
    global current_msg
    p_state = parsing_state.HEADER_POS0
    current_msg = message(0, 0, 0, 0, [], 0)


# Parse the incoming byte in the state machine
def parse_byte(byte):
    """
    Parse the incoming byte in the state machine. Sets the p_state global variable to the next state. 
    Sets the current_msg global variable to the parsed message as it becomes parsed.

    Args:
        byte: Byte to parse from the incoming byte stream.
    """
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
                current_msg) and current_msg.tgt_addr == my_addr:
            # CRC is good, send the message to the message handler
            notify_parsed_message(current_msg)
            reset_parsing_state()
        else:
            # CRC is bad, reset the parsing state machine
            reset_parsing_state()


# Parse an incoming list of bytes into the state machine
def parse_input_buffer(input_buffer):
    """
    Parse an incoming list of bytes into the state machine. This message simply takes a list and calls parse_byte on each byte.

    Args:
        input_buffer: List of bytes to parse.
    """
    try:
        # Parse the input buffer into the state machine
        for byte in input_buffer:
            parse_byte(byte)
    except Exception as e:
        print("Exception: " + str(e))


# Check for parsed messages in the queue
def check_for_parsed_messages():
    """
    Check for parsed messages in the queue. Returns a list of parsed messages. If no messages are available, returns an empty list.

    Returns:
        List of parsed messages.
    """
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
    """
    Build a message to send to the message handler. This method will calculate the CRC and return the message.

    Args:
        type: Message type.
        addr: Target address of the message.
        payload: Payload of the message.

    Returns:
        Message to send to the message handler.
    """
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

# Get the version information from the library
def get_version():
    """
    Get the version information from the library.

    Returns:
        Tuple of the major, minor, and revision version numbers.
    """
    return ver.VERSION_MAJOR, ver.VERSION_MINOR, ver.VERSION_REV

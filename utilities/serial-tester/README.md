# Python Serial Tester Program
## About
The serial tester program is a python implementation of the `embedded-serial-protocol` leveraging [pySerial](https://pyserial.readthedocs.io/en/latest/pyserial.html) to communicate to an embedded serial device over a USB comm port connection. The default configuration of the program allows the user to communicate with an arduino device running the `basic_example.ino` program located in the `examples/Arduino/sketches/basic_example` directory of this repository.
## Usage
To run the serial tester program simply run the following command (assuming bash shell):
```
python3 ./protocol_tester.py
```
Without any arguments you will recieve a print out of the help menu. The program can be run with all arguments passed from the command line however we recommend utilzing a `JSON` configuration file, of which an example file is provided here `.example_tester_configuration.json`. Feel free to modify this file or copy and modify to your needs. You can use multuple configuration files as long as they have seperate names. Pass in the configuration file to the programe like so:
```
python3 ./protocol_tester.py -c .example_tester_config.json
```
More details on the configuration file can be found below in the configuration file details section.
## Modes of operation
The serial tester program has three modes of operation:
1. `message` - Message mode is the default opmode of the serial tester program. If no opmode is specified at run time the program will attempt to use message mode. Message mode is utilized to send a single message and parse an expected response (if present). Message mode can be set by passing the opmode argument to the program `-o message`
2. `cmd_rsp` - Command response mode loads a list of commands and expected responses from the `command_response_pairs` list in the configuration file. The tester builds each command and response and sends to the command to the target device and awaits the specified response. Command response mode can be set by passing the opmode argument to the program `-o cmd_rsp`
3. `sniffer` - Sniffer mode passively monitors the comm port for incoming traffic and parses all traffic. This has yet to be implemented.
# Configuration File Details
## About
The configuration file is a json formatted file with section headers specifically named. You do not need a configuration file for message mode, but one is required for command response mode. All command line arguments can be overridden or placed directly in the configuration file.
## Serial Configuration
The serial configuration field is shown below, you can set the port name and baud rate of the comm port:
```
    "serial_config": {
        "port": "/dev/ttyACM0",
        "baud": 9600
    }
```
## Tester Configuration
The tester configuration field allows you to set the host address of the serial tester program and the target address outbound messages are going to:
```
    "tester_config": {
        "my_address": "0x01",
        "target_address": "0x1a"
    }
```
## Message Mode Configuration
Configure the basic message mode using the message config, and expected response config fields:
```
    "message_config": {
        "message_type": "0x0",
        "message_payload": []
    },
    "expected_rsp_config": {
        "message_type": "0x00",
        "message_payload": [
            "0x00",
            "0x01"
        ]
    }
```
## Command Response Configuration
The command response config field is required for command response mode to work properly. Configure the field as a list of command response pairs with their message types and payloads within each list field:
```
    "command_response_config": {
        "command_response_pairs": [
            {
                "command_type": "0x00",
                "response_type": "0x00",
                "command_payload": [],
                "response_payload": [
                    "0x00",
                    "0x01"
                ]
            },
            {
                "command_type": "0x01",
                "response_type": "0x01",
                "command_payload": [],
                "response_payload": []
            },
            {
                "command_type": "0x02",
                "response_type": "0x02",
                "command_payload": [
                    "0x45"
                ],
                "response_payload": []
            },
            {
                "command_type": "0x03",
                "response_type": "0x03",
                "command_payload": [],
                "response_payload": [
                    "0x45"
                ]
            },
            {
                "command_type": "0x04",
                "response_type": "0x04",
                "command_payload": [],
                "response_payload": [
                    "0x01",
                    "0x01",
                    "0x01",
                    "0x01"
                ]
            }
        ]
    }
```
# Example Calls & Output
## Setup
Each example shown below was performed using the `.example_tester_config.json` file in its current form and the `basic_example.ino` arduino sketch. Navigate to the `examples/Arduino/sketches/basic_example` directory for more information on how to setup an arduino board with the example program. Each sample was run in verbose mode to show full output.
## Message Mode
```
user:~/Repositories/embedded-serial-protocol$ python3 ./utilities/serial-tester/protocol_tester.py -c utilities/serial-tester/.example_tester_config.json --verbose
Parsed arguemtns:
Config file: utilities/serial-tester/.example_tester_config.json
Port:
Baud: 9600
Target address: -1
Message type: -1
Message payload: []
My address: -1
Verbose: True
Opmode: message
Expected response message type: -1
Expected response message payload: []

Config: {'serial_config': {'port': '/dev/ttyACM0', 'baud': 9600}, 'tester_config': {'my_address': '0x01', 'target_address': '0x1a'}, 'message_config': {'message_type': '0x0', 'message_payload': []}, 'expected_rsp_config': {'message_type': '0x00', 'message_payload': ['0x00', '0x01']}, 'command_response_config': {'command_response_pairs': [{'command_type': '0x00', 'response_type': '0x00', 'command_payload': [], 'response_payload': ['0x00', '0x01']}, {'command_type': '0x01', 'response_type': '0x01', 'command_payload': [], 'response_payload': []}, {'command_type': '0x02', 'response_type': '0x02', 'command_payload': ['0x45'], 'response_payload': []}, {'command_type': '0x03', 'response_type': '0x03', 'command_payload': [], 'response_payload': ['0x45']}, {'command_type': '0x04', 'response_type': '0x04', 'command_payload': [], 'response_payload': ['0x01', '0x01', '0x01', '0x01']}]}}
Overriding target address: 26
Overriding my address: 1
Overriding message type: 0
Overriding message payload: []
Overriding port: /dev/ttyACM0
Overriding baud: 9600
Overriding expected response message type: 0
Overriding expected response message payload: [0, 1]

Setting my address: 1
Opening serial port: /dev/ttyACM0 at baud 9600

Single message & respsone mode

Message: msg_type: 0x0, src_addr: 0x1, tgt_addr: 0x1a, msg_len: 0x0, msg_payload: [], msg_crc: 0xdee1, error: error_state.NO_ERROR
Raw message: [170, 85, 255, 1, 26, 0, 0, 0, 222, 225]

Expected response message: msg_type: 0x0, src_addr: 0x1a, tgt_addr: 0x1, msg_len: 0x2, msg_payload: [0, 1], msg_crc: 0xe200, error: error_state.NO_ERROR
Raw expected response message: [170, 85, 255, 26, 1, 0, 0, 2, 0, 1, 226, 0]

Raw bytes received: [170, 85, 255, 26, 1, 0, 0, 2, 0, 1, 226, 0]
Got 1 messages
Got expected response message!
Message: msg_type: 0x0, src_addr: 0x1a, tgt_addr: 0x1, msg_len: 0x2, msg_payload: [0, 1], msg_crc: 0xe200, error: error_state.NO_ERROR
Raw message: [170, 85, 255, 26, 1, 0, 0, 2, 0, 1, 226, 0]
Closing serial port
```
## Command Response Mode
```
user:~/Repositories/embedded-serial-protocol$ python3 ./utilities/serial-tester/protocol_tester.py -c utilities/serial-tester/.example_tester_config.json --verbose --opmode=cmd_rsp
Parsed arguemtns:
Config file: utilities/serial-tester/.example_tester_config.json
Port:
Baud: 9600
Target address: -1
Message type: -1
Message payload: []
My address: -1
Verbose: True
Opmode: cmd_rsp
Expected response message type: -1
Expected response message payload: []

Config: {'serial_config': {'port': '/dev/ttyACM0', 'baud': 9600}, 'tester_config': {'my_address': '0x01', 'target_address': '0x1a'}, 'message_config': {'message_type': '0x0', 'message_payload': []}, 'expected_rsp_config': {'message_type': '0x00', 'message_payload': ['0x00', '0x01']}, 'command_response_config': {'command_response_pairs': [{'command_type': '0x00', 'response_type': '0x00', 'command_payload': [], 'response_payload': ['0x00', '0x01']}, {'command_type': '0x01', 'response_type': '0x01', 'command_payload': [], 'response_payload': []}, {'command_type': '0x02', 'response_type': '0x02', 'command_payload': ['0x45'], 'response_payload': []}, {'command_type': '0x03', 'response_type': '0x03', 'command_payload': [], 'response_payload': ['0x45']}, {'command_type': '0x04', 'response_type': '0x04', 'command_payload': [], 'response_payload': ['0x01', '0x01', '0x01', '0x01']}]}}
Overriding target address: 26
Overriding my address: 1
Overriding message type: 0
Overriding message payload: []
Overriding port: /dev/ttyACM0
Overriding baud: 9600
Overriding expected response message type: 0
Overriding expected response message payload: [0, 1]

Setting my address: 1
Opening serial port: /dev/ttyACM0 at baud 9600

Command response mode

Overriding command type: 0
Overriding command payload: []
Overriding response type: 0
Overriding response payload: [0, 1]

Command: msg_type: 0x0, src_addr: 0x1, tgt_addr: 0x1a, msg_len: 0x0, msg_payload: [], msg_crc: 0xdee1, error: error_state.NO_ERROR
Raw command: [170, 85, 255, 1, 26, 0, 0, 0, 222, 225]

Expected response message: msg_type: 0x0, src_addr: 0x1a, tgt_addr: 0x1, msg_len: 0x2, msg_payload: [0, 1], msg_crc: 0xe200, error: error_state.NO_ERROR
Raw expected response message: [170, 85, 255, 26, 1, 0, 0, 2, 0, 1, 226, 0]

Overriding command type: 1
Overriding command payload: []
Overriding response type: 1
Overriding response payload: []

Command: msg_type: 0x1, src_addr: 0x1, tgt_addr: 0x1a, msg_len: 0x0, msg_payload: [], msg_crc: 0x1eb0, error: error_state.NO_ERROR
Raw command: [170, 85, 255, 1, 26, 1, 0, 0, 30, 176]

Expected response message: msg_type: 0x1, src_addr: 0x1a, tgt_addr: 0x1, msg_len: 0x0, msg_payload: [], msg_crc: 0xf8d2, error: error_state.NO_ERROR
Raw expected response message: [170, 85, 255, 26, 1, 1, 0, 0, 248, 210]

Overriding command type: 2
Overriding command payload: [69]
Overriding response type: 2
Overriding response payload: []

Command: msg_type: 0x2, src_addr: 0x1, tgt_addr: 0x1a, msg_len: 0x1, msg_payload: [69], msg_crc: 0x93df, error: error_state.NO_ERROR
Raw command: [170, 85, 255, 1, 26, 2, 0, 1, 69, 147, 223]

Expected response message: msg_type: 0x2, src_addr: 0x1a, tgt_addr: 0x1, msg_len: 0x0, msg_payload: [], msg_crc: 0xf822, error: error_state.NO_ERROR
Raw expected response message: [170, 85, 255, 26, 1, 2, 0, 0, 248, 34]

Overriding command type: 3
Overriding command payload: []
Overriding response type: 3
Overriding response payload: [69]

Command: msg_type: 0x3, src_addr: 0x1, tgt_addr: 0x1a, msg_len: 0x0, msg_payload: [], msg_crc: 0xde11, error: error_state.NO_ERROR
Raw command: [170, 85, 255, 1, 26, 3, 0, 0, 222, 17]

Expected response message: msg_type: 0x3, src_addr: 0x1a, tgt_addr: 0x1, msg_len: 0x1, msg_payload: [69], msg_crc: 0x86b9, error: error_state.NO_ERROR
Raw expected response message: [170, 85, 255, 26, 1, 3, 0, 1, 69, 134, 185]

Overriding command type: 4
Overriding command payload: []
Overriding response type: 4
Overriding response payload: [1, 1, 1, 1]

Command: msg_type: 0x4, src_addr: 0x1, tgt_addr: 0x1a, msg_len: 0x0, msg_payload: [], msg_crc: 0x1fa0, error: error_state.NO_ERROR
Raw command: [170, 85, 255, 1, 26, 4, 0, 0, 31, 160]

Expected response message: msg_type: 0x4, src_addr: 0x1a, tgt_addr: 0x1, msg_len: 0x4, msg_payload: [1, 1, 1, 1], msg_crc: 0x258c, error: error_state.NO_ERROR
Raw expected response message: [170, 85, 255, 26, 1, 4, 0, 4, 1, 1, 1, 1, 37, 140]

Sending message: msg_type: 0x0, src_addr: 0x1, tgt_addr: 0x1a, msg_len: 0x0, msg_payload: [], msg_crc: 0xdee1, error: error_state.NO_ERROR
Raw message: [170, 85, 255, 1, 26, 0, 0, 0, 222, 225]
Raw bytes received: [170, 85, 255, 26, 1, 0, 0, 2, 0, 1, 226, 0]
Got 1 messages
Got expected response message!
Message: msg_type: 0x0, src_addr: 0x1a, tgt_addr: 0x1, msg_len: 0x2, msg_payload: [0, 1], msg_crc: 0xe200, error: error_state.NO_ERROR
Raw message: [170, 85, 255, 26, 1, 0, 0, 2, 0, 1, 226, 0]

Sending message: msg_type: 0x1, src_addr: 0x1, tgt_addr: 0x1a, msg_len: 0x0, msg_payload: [], msg_crc: 0x1eb0, error: error_state.NO_ERROR
Raw message: [170, 85, 255, 1, 26, 1, 0, 0, 30, 176]
Raw bytes received: [170, 85, 255, 26, 1, 1, 0, 0, 248, 210]
Got 1 messages
Got expected response message!
Message: msg_type: 0x1, src_addr: 0x1a, tgt_addr: 0x1, msg_len: 0x0, msg_payload: [], msg_crc: 0xf8d2, error: error_state.NO_ERROR
Raw message: [170, 85, 255, 26, 1, 1, 0, 0, 248, 210]

Sending message: msg_type: 0x2, src_addr: 0x1, tgt_addr: 0x1a, msg_len: 0x1, msg_payload: [69], msg_crc: 0x93df, error: error_state.NO_ERROR
Raw message: [170, 85, 255, 1, 26, 2, 0, 1, 69, 147, 223]
Raw bytes received: [170, 85, 255, 26, 1, 2, 0, 0, 248, 34]
Got 1 messages
Got expected response message!
Message: msg_type: 0x2, src_addr: 0x1a, tgt_addr: 0x1, msg_len: 0x0, msg_payload: [], msg_crc: 0xf822, error: error_state.NO_ERROR
Raw message: [170, 85, 255, 26, 1, 2, 0, 0, 248, 34]

Sending message: msg_type: 0x3, src_addr: 0x1, tgt_addr: 0x1a, msg_len: 0x0, msg_payload: [], msg_crc: 0xde11, error: error_state.NO_ERROR
Raw message: [170, 85, 255, 1, 26, 3, 0, 0, 222, 17]
Raw bytes received: [170, 85, 255, 26, 1, 3, 0, 1, 69, 134, 185]
Got 1 messages
Got expected response message!
Message: msg_type: 0x3, src_addr: 0x1a, tgt_addr: 0x1, msg_len: 0x1, msg_payload: [69], msg_crc: 0x86b9, error: error_state.NO_ERROR
Raw message: [170, 85, 255, 26, 1, 3, 0, 1, 69, 134, 185]

Sending message: msg_type: 0x4, src_addr: 0x1, tgt_addr: 0x1a, msg_len: 0x0, msg_payload: [], msg_crc: 0x1fa0, error: error_state.NO_ERROR
Raw message: [170, 85, 255, 1, 26, 4, 0, 0, 31, 160]
Raw bytes received: [170, 85, 255, 26, 1, 5, 0, 0, 57, 147]
Got 1 messages
Error: Message type mismatch. Expected: 0x4, Actual: 0x5
Raw message: [170, 85, 255, 26, 1, 5, 0, 0, 57, 147]
```

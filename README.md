# embedded-serial-protocol
Boiler plate implementation, testing, and documentation for a common node to node, or multi node serial communication bus.
## Overview
The inspiration for this project came from many implementations of similar serial protocols over the years. I wanted to create a simple but reliable communication protocol and testing framework that could be easily customized, tested, and implemented without having to write the same code and documentation over and over again.This code is under MIT license, and such it is free to use and available for anyone to create / modify a closed source version of the protocol. I hope it serves you well.
## Where is Everything?
This project is broken into three seperate top level directories:
1. `protocol-implementaion` - In the implementation directory you will find implementations for various programming languages that have been added over time. Cross language compatibility is tested for in all release versions. You will also find unit tests available in each implementation directory along with instructions on how to run each unit test suite. All releases are passing unit tests as long as you can reproduce the setup in the repo.
2. `examples` - Example implementations for various platforms along with instructions on project setup / usage.
3. `utilities` - Utilities for testing protocol implementaions and other helpful tools that I have used for development / validation of the protocol. In particular the python serial-tester is a super helpful tool for getting embedded devices validated quickly.
## Bugs, Feature Requests, and Other
If you find a bug, have a feature request, or want to contribute feel free to file an issue on the github repository, or make a PR into the project. As of now I am the sole maintener of the project, but I do actively develop for this project and utilize it in my professional career. If you wish to make a closed source version of this project simply fork the repo and modify as you wish.
# Details
Below are the architectural details and technical implementation of the serial protocol.
## Message Format
Below is the basic message format followed by all implementations. There are a handful of architecture implementations that arise from this format which are listed below the table.
| Byte Position | Field Name           | Value        |
|---------------|----------------------|--------------|
| 0             | Header 0             | 0xAA         |
| 1             | Header 1             | 0x55         |
| 2             | Header 2             | 0xFF         |
| 3             | Source Address       | --           |
| 4             | Target Address       | --           |
| 5             | Message Type         | --           |
| 6             | Payload Length (MSB) | n >> 8       |
| 7             | Payload length (MSB) | (n & 0xFF)   |
| ...           | Payload Start        | --           |
| (8 + n)       | CRC16 (MSB)          | crc >> 8     |
| (9 + n)       | CRC16 (LSB)          | (crc & 0xFF) |
1. There are a maximum of 256 (including address 0) addressed allowed on a single bus. 
2. There are a maximum of 256 message types per address allowed in the protocol.
3. The maximum payload length is 65535 bytes.
If the above limitations do not work for you, then you must change the message structure and adjust the implementations accordingly.
## Parsing
The parsing state machines are designed to parse a single byte at a time, all parsing state machines are wrapped in a method called `parse_input_buffer` which passes a buffer of bytes (uint8_t) and the maximum number of allowable bytes to parse, which then calls the parser and parses one byte at a time. The parser parses _all_ traffic on the bus, even if the message is not meant for the parsing device. This is to avoid unintentional parsing of byte sequences outside of the valid transmission format. Devices will only call the user_rx callback if a message is parsed successfully and it is destined for the target device.
## Throughput
Due to the implementation of the parsing state machine, this protocol is a one to many half duplex communication scheme. In which all devices using the protocol are able to transmit and receive. All transmissions are broadcast and parsed by all nodes on the bus.

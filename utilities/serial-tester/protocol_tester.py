import getopt
import json
from socket import timeout
import sys
import time
import serial
import importlib.util

# Compare a message against an expected message
def compare_message(msg, expected_msg):
    if msg.msg_type != expected_msg.msg_type:
        print("Error: Message type mismatch. Expected: {}, Actual: {}".format(
            hex(expected_msg.msg_type), hex(msg.msg_type)))
        return False
    if msg.src_addr != expected_msg.src_addr:
        print("Error: Source address mismatch. Expected: {}, Actual: {}".format(
            hex(expected_msg.src_addr), hex(msg.src_addr)))
        return False
    if msg.tgt_addr != expected_msg.tgt_addr:
        print("Error: Target address mismatch. Expected: {}, Actual: {}".format(
            hex(expected_msg.tgt_addr), hex(msg.tgt_addr)))
        return False
    if msg.msg_len != expected_msg.msg_len:
        print("Error: Message length mismatch. Expected: {}, Actual: {}".format(
            hex(expected_msg.msg_len), hex(msg.msg_len)))
        return False
    if msg.msg_payload != expected_msg.msg_payload:
        print("Error: Message payload mismatch. Expected: {}, Actual: {}".format(
            expected_msg.msg_payload, msg.msg_payload))
        return False
    if msg.msg_crc != expected_msg.msg_crc:
        print("Error: Message CRC mismatch. Expected: {}, Actual: {}".format(
            hex(expected_msg.msg_crc), hex(msg.msg_crc)))
        return False
    return True

# Send a message through a serial port
def send_message(ser, msg):
    try:
        # Send the message
        ser.write(msg)
    except Exception as e:
        print("Error: " + str(e))
        sys.exit(2)


# Read from the serial port
def receive_message(ser, length):
    try:
        # Read from the serial port
        buf = ser.read(length)
        return buf
    except Exception as e:
        print("Error: " + str(e))
        sys.exit(2)


# Helper function to import a module from a file.
def module_from_file(module_name, file_path):
    spec = importlib.util.spec_from_file_location(module_name, file_path)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


# Open the serial port
def open_serial_port(port, baud, delay_seconds=0):
    try:
        ser = serial.Serial(port, baud)
        ser.timeout = 1
        time.sleep(delay_seconds)
        return ser
    except Exception as e:
        print("Error: " + str(e))
        sys.exit(2)


# Parse the objects from the configuration file
def get_config_field(config, key):
    if key in config:
        return config[key]
    else:
        return None


# Load the configuration file
def load_config(config_file):
    try:
        # Open the configuration file
        f = open(config_file, "r")
        buf = f.read()
        f.close()

        # unpack the json object
        config = json.loads(buf)
        return config
    except Exception as e:
        print("Error: " + str(e))
        sys.exit(2)


# Print the usage of the program.
def print_usage():
    print("Usage: protocol_tester.py [options]")
    print("Options:")
    print("-h, --help: Print this help message.")
    print("-c, --config_file: The configuration file to use.")
    print("-p, --port: The port to use.")
    print("-b, --baud: The baud rate to use.")
    print("-t, --target_address: The target address to use.")
    print("-m, --message_type: The message type to use.")
    print("-p, --message_payload: The message payload to use.")
    print("-a, --my_address: The my address to use.")
    print("-v, --verbose: Print verbose output.")
    print("-o, --opmode: The opmode to use.")
    print("-e", "--response_type: The expected response message type.")
    print("-r", "--response_payload: The expected response message payload.")


# Main function for the protocol tester.
def main(argv):
    # Check the argument length
    if len(argv) < 1:
        print("At least one argument is required.")
        print_usage()
        sys.exit(2)

    # Argument list
    short_options = "hc:p:b:t:m:p:a:v:o:e:r"
    long_options = [
        "help", "config_file=", "port=", "baud=", "target_address=",
        "message_type=", "message_payload=", "my_address=", "verbose",
        "opmode=", "response_type=", "response_payload="
    ]
    try:
        opts, args = getopt.getopt(argv, short_options, long_options)
    except getopt.GetoptError:
        # print the exception
        print(sys.exc_info()[1])
        print_usage()
        sys.exit(2)

    # Set arguments first
    config_file = ""
    port = ""
    baud = 9600
    target_address = -1
    message_type = -1
    message_payload = []
    my_address = -1
    verbose = False
    opmode = "message"
    exp_rsp_message_type = -1
    exp_rsp_message_payload = []

    # Configure the arguments
    for opt, arg in opts:
        if opt in ("-h", "--help"):
            print_usage()
            sys.exit()
        elif opt in ("-c", "--config_file"):
            config_file = arg
        elif opt in ("-p", "--port"):
            port = arg
        elif opt in ("-b", "--baud"):
            baud = arg
        elif opt in ("-t", "--target_address"):
            target_address = arg
        elif opt in ("-m", "--message_type"):
            message_type = arg
        elif opt in ("-p", "--message_payload"):
            message_payload = arg
        elif opt in ("-a", "--my_address"):
            my_address = arg
        elif opt in ("-v", "--verbose"):
            verbose = True
        elif opt in ("-o", "--opmode"):
            opmode = arg
        elif opt in ("-e", "--response_type"):
            exp_rsp_message_type = arg
        elif opt in ("-r", "--response_payload"):
            exp_rsp_message_payload = arg

    # Print the arguments if verbose is enabled
    if verbose:
        print("Parsed arguemtns:")
        print("Config file: " + config_file)
        print("Port: " + port)
        print("Baud: " + str(baud))
        print("Target address: " + str(target_address))
        print("Message type: " + str(message_type))
        print("Message payload: " + str(message_payload))
        print("My address: " + str(my_address))
        print("Verbose: " + str(verbose))
        print("Opmode: " + opmode)
        print("Expected response message type: " + str(exp_rsp_message_type))
        print("Expected response message payload: " + str(exp_rsp_message_payload))
        print("")

    # Parse the configuration file if it is specified
    if config_file != "":
        config = load_config(config_file)
        if verbose:
            print("Config: " + str(config))

        # Override any arguments with the configuration file
        tester_config = get_config_field(config, "tester_config")
        message_config = get_config_field(config, "message_config")
        serial_config = get_config_field(config, "serial_config")
        expected_rsp_config = get_config_field(config, "expected_rsp_config")

        # Load the tester configuration options
        if tester_config != None:
            # Convert the hex strings to ints
            target_address = int(
                get_config_field(tester_config, "target_address"), 16)
            my_address = int(get_config_field(tester_config, "my_address"), 16)
            if verbose:
                print("Overriding target address: " + str(target_address))
                print("Overriding my address: " + str(my_address))

        # Load the message configuration options
        if message_config != None:
            # Convert the hex strings to ints
            message_type = int(
                get_config_field(message_config, "message_type"), 16)
            # Convert the payload from a hex string to a list of ints
            message_payload = [
                int(x, 16)
                for x in get_config_field(message_config, "message_payload")
            ]
            if verbose:
                print("Overriding message type: " + str(message_type))
                print("Overriding message payload: " + str(message_payload))

        # Load the serial configuration options
        if serial_config != None:
            port = get_config_field(serial_config, "port")
            baud = get_config_field(serial_config, "baud")
            if verbose:
                print("Overriding port: " + str(port))
                print("Overriding baud: " + str(baud))

        # Load the expected response configuration options
        if expected_rsp_config != None:
            # Convert the hex strings to ints
            exp_rsp_message_type = int(
                get_config_field(expected_rsp_config, "message_type"), 16)
            # Convert the payload from a hex string to a list of ints
            exp_rsp_message_payload = [
                int(x, 16)
                for x in get_config_field(expected_rsp_config, "message_payload")
            ]
            if verbose:
                print("Overriding expected response message type: " + str(exp_rsp_message_type))
                print("Overriding expected response message payload: " + str(exp_rsp_message_payload))

        # Give an extra space if we are verbose
        if verbose:
            print("")

    # Load the protocol module
    protocol = module_from_file("protocol",
                                "protocol-implementation/Python/protocol.py")
    
    # Set "my address"
    if verbose:
        print("Setting my address: " + str(my_address))
    protocol.set_my_address(my_address)

    # Attempt to open the  serial port
    if verbose:
        print("Opening serial port: " + port + " at baud " + str(baud))
        print("")
    ser = open_serial_port(port, baud, 10)

    if opmode == "message":
        # Build the message
        msg = protocol.build_message(message_type, target_address,
                                     message_payload)
        if verbose:
            print("Message: " + str(msg))
            print("Raw message: " + str(msg.to_list()))
            print("")

        # Build the expected response message
        exp_rsp_msg = protocol.build_message(exp_rsp_message_type, my_address,exp_rsp_message_payload)
        if verbose:
            print("Expected response message: " + str(exp_rsp_msg))
            print("Raw expected response message: " + str(exp_rsp_msg.to_list()))
            print("")

        # Send the message
        send_message(ser, msg.to_list())

        # Receive the message
        rx_buf = receive_message(ser, 1024)
        # Convert the bytes int a list of ints
        rx_buf = [x for x in rx_buf]

        if verbose:
            print("Raw bytes received: " + str(rx_buf))

        # Parse the message
        protocol.parse_input_buffer(rx_buf)
        rx_msg = protocol.check_for_parsed_messages()
        if rx_msg != None:
            if verbose:
                print("Got " + str(len(rx_msg)) + " messages")
            
            # Loop through the messages and see if we got the expected response message
            for msg in rx_msg:
                if verbose:
                    print("Message: " + str(msg))
                    print("Raw message: " + str(msg.to_list()))
                    print("")
                if compare_message(msg, exp_rsp_msg):
                    print("Got expected response message!")
                    break

        # For now close the serial port before exiting
        if verbose:
            print("Closing serial port")
            print("")
        ser.close()
        sys.exit()


# Main caller
if __name__ == "__main__":
    main(sys.argv[1:])
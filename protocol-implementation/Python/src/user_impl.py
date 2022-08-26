import protocol as prot

# User implementation of message handler
def user_handle_message(msg):
    if isinstance(msg, prot.message):
        print("Message received: {}".format(msg))
    else:
        print("Message is not a valid message")
    
    # TODO: User code to handle the message here

# User implementation of message sender
def user_send_message(msg):
    if isinstance(msg, prot.message):
        print("Sending message: {}".format(msg))
    else:
        print("Message is not a valid message")

    # TODO: User code to send the message here
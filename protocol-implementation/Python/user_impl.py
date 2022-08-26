import protocol as prot

rx_notify = False
tx_notify = False


# Setters of the global rx / tx notify flags
def user_set_rx_notify(notify):
    global rx_notify
    rx_notify = notify

    # TODO: User implementation for rx_notify here


# Setters of the global rx / tx notify flags
def user_set_tx_notify(notify):
    global tx_notify
    tx_notify = notify

    # TODO: User implementation for tx_notify here


# User get notify function for rx
def user_get_rx_notify():
    return rx_notify


# User get notify function for tx
def user_get_tx_notify():
    return tx_notify


# User implementation of message handler
def user_handle_message(msg):
    user_set_rx_notify(True)

    if isinstance(msg, prot.message):
        #print("Message received: {}".format(msg))
        print("Message received")
    else:
        print("Message is not a valid message")

    # TODO: User code to handle the message here


# User implementation of message sender
def user_send_message(msg):
    user_set_tx_notify(True)

    if isinstance(msg, prot.message):
        #print("Sending message: {}".format(msg))
        print("Sending message")
    else:
        print("Message is not a valid message")

    # TODO: User code to send the message here
from logging import raiseExceptions
import protocol as prot

rx_notify = False
tx_notify = False

# Set to true if you want debug prints
DEBUG = False


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
    # Throw an exception if this gets called with an improper message type
    if not isinstance(msg, prot.message):
        if DEBUG:
            print("Message is not a valid message")
        raise Exception("Message {} not a valid message", msg)
    elif DEBUG == True:
        print("Message received: {}".format(msg))

    user_set_rx_notify(True)

    # TODO: User code to receive the message here


# User implementation of message sender
def user_send_message(msg):
    # Throw an exception if this gets called with an improper message type
    if not isinstance(msg, prot.message):
        if DEBUG:
            print("Message is not a valid message")
        raise Exception("Message {} not a valid message", msg)
    elif DEBUG == True:
        print("Message sent: {}".format(msg))

    user_set_tx_notify(True)

    # TODO: User code to send the message here

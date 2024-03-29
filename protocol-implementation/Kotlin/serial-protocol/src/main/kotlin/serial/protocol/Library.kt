
/*DISCLAIMER:

This code is protected under the MIT open source license. The code is provided
"as is" without warranty of any kind, either express or implied, including but
not limited to the implied warranties of merchantability, fitness for a particular
purpose, or non-infringement. In no event shall the author or any other party be
liable for any direct, indirect, incidental, special, exemplary, or consequential
damages, however caused and on any theory of liability, whether in contract,
strict liability, or tort (including negligence or otherwise), arising in any way
out of the use of this code or performance or use of the results of this code. By
using this code, you agree to hold the author and any other party harmless from
any and all liability and to use the code at your own risk.

This code was written by GitHub user: budgettsfrog
Contact: budgettsfrog@protonmail.com
GitHub: https://github.com/warrenwoolseyiii*/

/*
 * This Kotlin source file was generated by the Gradle 'init' task.
 */
package serial.protocol

import Version.VERSION_MAJOR
import Version.VERSION_MINOR
import Version.VERSION_REV

class CommsProtocol {

    /**
     * Companion object to hold constants
     */
    companion object {
        const val HEADER_POS_0 = 0
        const val HEADER_POS_1 = 1
        const val HEADER_POS_2 = 2
        const val SRC_ADDRESS_POS = 3
        const val TGT_ADDRESS_POS = 4
        const val MSG_TYPE_POS = 5
        const val PAYLOAD_LENGTH_MSB_POS = 6
        const val PAYLOAD_LENGTH_LSB_POS = 7
        const val PAYLOAD_POS = 8
        const val HEADER_0 = 0xAA.toByte()
        const val HEADER_1 = 0x55.toByte()
        const val HEADER_2 = 0xFF.toByte()
        const val MAX_PAYLOAD_LENGTH = 65535
        const val NUM_OVERHEAD_BYTES = 10
    }

    /**
     * Data class to represent a message
     * @param srcAddress Source address of the message (coming from)
     * @param tgtAddress Target address of the message (going to)
     * @param msgType Message type of the message
     * @param payloadLength Length of the payload in bytes
     * @param payload Payload of the message
     * @param crc CRC of the message
     */
    data class Message(
            val srcAddress: Byte,
            val tgtAddress: Byte,
            val msgType: Byte,
            val payloadLength: Short,
            val payload: ByteArray,
            val crc: Short
    ) {

        /**
         * Method to convert message to byte array
         * @return ByteArray object containing the message
         */
        fun toByteArray(): ByteArray {
            val byteArray = ByteArray(payloadLength + NUM_OVERHEAD_BYTES)
            byteArray[HEADER_POS_0] = HEADER_0
            byteArray[HEADER_POS_1] = HEADER_1
            byteArray[HEADER_POS_2] = HEADER_2
            byteArray[SRC_ADDRESS_POS] = srcAddress
            byteArray[TGT_ADDRESS_POS] = tgtAddress
            byteArray[MSG_TYPE_POS] = msgType
            byteArray[PAYLOAD_LENGTH_MSB_POS] = (payloadLength.toInt() shr 8).toByte()
            byteArray[PAYLOAD_LENGTH_LSB_POS] = payloadLength.toByte()
            for (i in 0 until payloadLength) {
                byteArray[PAYLOAD_POS + i] = payload[i]
            }
            byteArray[byteArray.size - 2] = (crc.toInt() shr 8).toByte()
            byteArray[byteArray.size - 1] = crc.toByte()
            return byteArray
        }

        /**
         * Method to convert byte array to message
         * @param byteArray ByteArray object containing the message
         * @return Message object containing the message
         */
        fun fromByteArray(byteArray: ByteArray): Message {
            val srcAddress = byteArray[SRC_ADDRESS_POS]
            val tgtAddress = byteArray[TGT_ADDRESS_POS]
            val msgType = byteArray[MSG_TYPE_POS]
            val payloadLength =
                    ((byteArray[PAYLOAD_LENGTH_MSB_POS].toInt() shl 8) or
                                    byteArray[PAYLOAD_LENGTH_LSB_POS].toInt())
                            .toShort()
            val payload = ByteArray(payloadLength.toInt())
            for (i in 0 until payloadLength) {
                payload[i] = byteArray[PAYLOAD_POS + i]
            }
            val crc = ((byteArray[byteArray.size - 2].toInt() shl 8) or byteArray[byteArray.size - 1].toInt()).toShort()
            return Message(srcAddress, tgtAddress, msgType, payloadLength, payload, crc)
        }

        /**
         * Method to calculate the CRC of this message
         * @return CRC of this message
         */
        fun calculateCrc(): Short {
            val array = toByteArray()
            var crc = 0xFFFF
            for (i in 0 until array.size - 2) {
                crc = (crc.toInt() xor (array[i].toInt() and 0xFF))
                for (j in 0 until 8) {
                    if (crc.toInt() and 0x0001 != 0) {
                        crc = ((crc.toInt() shr 1) xor 0xA001)
                    } else {
                        crc = (crc.toInt() shr 1)
                    }
                }
            }
            return crc.toShort()
        }

        /**
         * Method to print the message to the console
         */
        fun printMsg() {
            println("srcAddress: $srcAddress")
            println("tgtAddress: $tgtAddress")
            println("msgType: $msgType")
            println("payloadLength: $payloadLength")
            for (i in 0 until payloadLength) {
                println("payload[$i]: ${payload[i]}")
            }
            println("crc: $crc")
        }
    }

    // Public fields
    var myAddr: Byte = 0x00.toByte()
    var broadcastAddr: Byte = 0xFF.toByte()
    var ignoreSrcAddr: Boolean = false

    // Public logging output
    var logEnabled: Boolean = false

    // Private parsing state machine fields
    private var state = 0
    private var currentTgtAddr: Byte = 0x00.toByte()
    private var currentSrcAddr: Byte = 0x00.toByte()
    private var currentMsgType: Byte = 0x00.toByte()
    private var currentPayloadLength: Short = 0
    private var currentPayload: ByteArray = ByteArray(0)
    private var currentPayloadNdx = 0
    private var currentCrc: Short = 0

    /**
     * Method to log output to the console or other output of the implementer's choice
     * @param msg Message to log in string format
     */
    fun log(msg: String) {
        if (logEnabled) {
            println(msg)
        }
    }

    /**
     * Method to create a message
     * @param tgtAddress Target address of the message (going to)
     * @param msgType Message type of the message
     * @param payload Payload of the message
     * @return Message object containing the message
     */
    fun createMessage(tgtAddress: Byte, msgType: Byte, payload: ByteArray): Message {
        // Check the payload length and throw an exception if it is too long
        if (payload.size > MAX_PAYLOAD_LENGTH) {
            throw Exception("Payload length cannot exceed $MAX_PAYLOAD_LENGTH bytes")
        }
        val payloadLength = payload.size.toShort()
        val srcAddress = myAddr
        val crc = Message(srcAddress, tgtAddress, msgType, payloadLength, payload, 0).calculateCrc()
        val msg = Message(srcAddress, tgtAddress, msgType, payloadLength, payload, crc)
        log("Created message:")
        log(msg.toString())
        return msg
    }

    /**
     * Method to parse a byte array into a list of messages - use this function to parse all incoming bytes
     * from the communication channel.
     * @param byteArray ByteArray object containing raw bytes from the communication channel
     * @param len Length of the byte array
     * @return ArrayList of Message objects containing the messages, which may be empty if no messages were found
     */
    fun parseMessage(byteArray: ByteArray, len: Int): ArrayList<Message> {
        val messages = ArrayList<Message>()

        // Make sure the length is not greater than the byte array size
        var length = len
        if (length > byteArray.size) {
            length = byteArray.size
        }

        // Parsing state machine looks at the header bytes first
        for (i in 0 until length) {
            log("Parsing byte ${byteArray[i]}")
            log("State is $state")
            when (state) {
                HEADER_POS_0 -> {
                    if (byteArray[i] == HEADER_0) {
                        state = 1
                    }
                }
                HEADER_POS_1 -> {
                    if (byteArray[i] == HEADER_1) {
                        state = 2
                    } else {
                        state = 0
                    }
                }
                HEADER_POS_2 -> {
                    if (byteArray[i] == HEADER_2) {
                        state = 3
                    } else {
                        state = 0
                    }
                }
                SRC_ADDRESS_POS -> {
                    currentSrcAddr = byteArray[i]
                    state = 4
                }
                TGT_ADDRESS_POS -> {
                    currentTgtAddr = byteArray[i]
                    state = 5
                }
                MSG_TYPE_POS -> {
                    currentMsgType = byteArray[i]
                    state = 6
                }
                PAYLOAD_LENGTH_MSB_POS -> {
                    currentPayloadLength = (byteArray[i].toInt() shl 8).toShort()
                    state = 7
                }
                PAYLOAD_LENGTH_LSB_POS -> {
                    currentPayloadLength =
                            (currentPayloadLength.toInt() or byteArray[i].toInt()).toShort()
                    currentPayload = ByteArray(currentPayloadLength.toInt())
                    if (currentPayloadLength == 0.toShort()) {
                        state = 9
                    } else {
                        state = 8
                    }
                }
                PAYLOAD_POS -> {
                    currentPayload[currentPayloadNdx++] = byteArray[i]
                    if (currentPayloadNdx == currentPayloadLength.toInt()) {
                        state = 9
                    }
                }
                PAYLOAD_POS + 1 -> {
                    currentCrc = (byteArray[i].toInt() shl 8).toShort()
                    state = 10
                }
                PAYLOAD_POS + 2 -> {
                    currentCrc = (currentCrc.toInt() or (byteArray[i].toInt() and 0xFF)).toShort()
                    val msg =
                            Message(
                                    srcAddress = currentSrcAddr,
                                    tgtAddress = currentTgtAddr,
                                    msgType = currentMsgType,
                                    payloadLength = currentPayloadLength,
                                    payload = currentPayload,
                                    crc = currentCrc
                            )
                    if (msg.calculateCrc() == currentCrc) {
                        log("Recieved valid message:")
                        if (msg.tgtAddress == myAddr ||
                                        msg.tgtAddress == broadcastAddr ||
                                        ignoreSrcAddr
                        ) {
                            log(msg.toString())
                            messages.add(msg)
                        }
                    } else {
                        log(
                                "Message failed CRC check, expected ${msg.calculateCrc()} but got $currentCrc"
                        )
                    }
                    resetParser()
                }
            }
        }
        return messages
    }

    /**
     * Method to reset the parser state machine
     */
    fun resetParser() {
        state = 0
        currentSrcAddr = 0x00.toByte()
        currentTgtAddr = 0x00.toByte()
        currentMsgType = 0x00.toByte()
        currentPayloadLength = 0
        currentPayload = ByteArray(0)
        currentPayloadNdx = 0
        currentCrc = 0
    }

    /**
     * Method to get the version of the library
     * @return String containing the version of the library
     */
    fun getVersion(): String {
        return "$VERSION_MAJOR.$VERSION_MINOR.$VERSION_REV"
    }
}

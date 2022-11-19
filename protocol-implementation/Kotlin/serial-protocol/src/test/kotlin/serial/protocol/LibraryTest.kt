/*
 * This Kotlin source file was generated by the Gradle 'init' task.
 */
package serial.protocol

import kotlin.test.Test
import kotlin.test.assertTrue

class LibraryTest {
    // Test function to set and get myAddr
    @Test fun testMyAddr() {
        val classUnderTest = CommsProtocol()
        classUnderTest.myAddr = 0x01.toByte()
        assertTrue(classUnderTest.myAddr == 0x01.toByte(), "myAddr should be 0x01")
    }

    // Test function to set and get the broadcast address
    @Test fun testBroadcastAddr() {
        val classUnderTest = CommsProtocol()
        classUnderTest.broadcastAddr = 0xFF.toByte()
        assertTrue(classUnderTest.broadcastAddr == 0xFF.toByte(), "broadcastAddr should be 0xFF")
    }

    // Test function to generate a basic message
    @Test fun testGenerateMessage() {
        val classUnderTest = CommsProtocol()
        val msg = classUnderTest.createMessage(tgtAddress = 0x02.toByte(), msgType = 0x01.toByte(), payload = byteArrayOf(0x01.toByte(), 0x02.toByte()), isBroadcast = false)
        assertTrue(msg.srcAddress == classUnderTest.myAddr, "srcAddress should be myAddr")
        assertTrue(msg.tgtAddress == 0x02.toByte(), "tgtAddress should be 0x02")
        assertTrue(msg.msgType == 0x01.toByte(), "msgType should be 0x01")
        assertTrue(msg.payloadLength == 2.toShort(), "payloadLength should be 2")
        assertTrue(msg.payload[0] == 0x01.toByte(), "payload[0] should be 0x01")
        assertTrue(msg.payload[1] == 0x02.toByte(), "payload[1] should be 0x02")
    }

    // Test function to generate a broadcast message
    @Test fun testGenerateBroadcastMessage() {
        val classUnderTest = CommsProtocol()
        val msg = classUnderTest.createMessage(tgtAddress = classUnderTest.broadcastAddr, msgType = 0x01.toByte(), payload = byteArrayOf(0x01.toByte(), 0x02.toByte()), isBroadcast = true)
        assertTrue(msg.srcAddress == classUnderTest.myAddr, "srcAddress should be myAddr")
        assertTrue(msg.tgtAddress == classUnderTest.broadcastAddr, "tgtAddress should be broadcastAddr")
        assertTrue(msg.msgType == 0x01.toByte(), "msgType should be 0x01")
        assertTrue(msg.payloadLength == 2.toShort(), "payloadLength should be 2")
        assertTrue(msg.payload[0] == 0x01.toByte(), "payload[0] should be 0x01")
        assertTrue(msg.payload[1] == 0x02.toByte(), "payload[1] should be 0x02")
    }

    // Test function to generate a message with no payload
    @Test fun testGenerateMessageNoPayload() {
        val classUnderTest = CommsProtocol()
        val msg = classUnderTest.createMessage(tgtAddress = 0x02.toByte(), msgType = 0x01.toByte(), payload = byteArrayOf(), isBroadcast = false)
        assertTrue(msg.srcAddress == classUnderTest.myAddr, "srcAddress should be myAddr")
        assertTrue(msg.tgtAddress == 0x02.toByte(), "tgtAddress should be 0x02")
        assertTrue(msg.msgType == 0x01.toByte(), "msgType should be 0x01")
        assertTrue(msg.payloadLength == 0.toShort(), "payloadLength should be 0")
    }

    // Test function to generate a message with the largest possible payload
    @Test fun testGenerateMessageMaxPayload() {
        val classUnderTest = CommsProtocol()
        val msg = classUnderTest.createMessage(tgtAddress = 0x02.toByte(), msgType = 0x01.toByte(), payload = ByteArray(65535), isBroadcast = false)
        assertTrue(msg.srcAddress == classUnderTest.myAddr, "srcAddress should be myAddr")
        assertTrue(msg.tgtAddress == 0x02.toByte(), "tgtAddress should be 0x02")
        assertTrue(msg.msgType == 0x01.toByte(), "msgType should be 0x01")
        assertTrue(msg.payloadLength == 65535.toShort(), "payloadLength should be 65535")
    }

    // Test function to generate a message with a payload that is too large
    @Test fun testGenerateMessageTooLargePayload() {
        val classUnderTest = CommsProtocol()
        // This should throw an exception
        try {
            classUnderTest.createMessage(tgtAddress = 0x02.toByte(), msgType = 0x01.toByte(), payload = ByteArray(65536), isBroadcast = false)
            // If we get here then the test failed
            assertTrue(false, "Payload too large should throw an exception")
        } catch (e: Exception) {
            // Print the exception message
            println(e.message)
            // Return true if the exception was thrown
            assertTrue(true, "Exception should be thrown")
        }
    }

    // Test that the message is correctly formatted in a byte array
    @Test fun testMessageToByteArray() {
        val classUnderTest = CommsProtocol()
        val msg = classUnderTest.createMessage(tgtAddress = 0x02.toByte(), msgType = 0x01.toByte(), payload = byteArrayOf(0x01.toByte(), 0x02.toByte()), isBroadcast = false)
        val msgBytes = msg.toByteArray()
        val crc = msg.crc;
        
        // Check the header, values are constants in the companion class of the unit under test
        assertTrue(msgBytes[CommsProtocol.HEADER_POS_0] == CommsProtocol.HEADER_0, "msgBytes[0] should be HEADER_0")
        assertTrue(msgBytes[CommsProtocol.HEADER_POS_1] == CommsProtocol.HEADER_1, "msgBytes[1] should be HEADER_1")
        assertTrue(msgBytes[CommsProtocol.HEADER_POS_2] == CommsProtocol.HEADER_2, "msgBytes[2] should be HEADER_2")
        assertTrue(msgBytes[CommsProtocol.SRC_ADDRESS_POS] == classUnderTest.myAddr, "msgBytes[3] should be myAddr")
        assertTrue(msgBytes[CommsProtocol.TGT_ADDRESS_POS] == 0x02.toByte(), "msgBytes[4] should be 0x02")
        assertTrue(msgBytes[CommsProtocol.MSG_TYPE_POS] == 0x01.toByte(), "msgBytes[5] should be 0x01")
        assertTrue(msgBytes[CommsProtocol.PAYLOAD_LENGTH_MSB_POS] == 0x00.toByte(), "msgBytes[6] should be 0x00")
        assertTrue(msgBytes[CommsProtocol.PAYLOAD_LENGTH_LSB_POS] == 0x02.toByte(), "msgBytes[7] should be 0x02")
        assertTrue(msgBytes[CommsProtocol.PAYLOAD_POS] == 0x01.toByte(), "msgBytes[8] should be 0x01")
        assertTrue(msgBytes[CommsProtocol.PAYLOAD_POS + 1] == 0x02.toByte(), "msgBytes[9] should be 0x02")
        assertTrue(msgBytes[CommsProtocol.PAYLOAD_POS + 2] == (crc.toInt() shr 8).toByte(), "msgBytes[10] should be crc rsh 8")
        assertTrue(msgBytes[CommsProtocol.PAYLOAD_POS + 3] == (crc.toInt() and 0xFF).toByte(), "msgBytes[11] should be crc and 0xFF")
    }

    // Test that the message is correctly formatted in a byte array with no payload
    @Test fun testMessageToByteArrayNoPayload() {
        val classUnderTest = CommsProtocol()
        val msg = classUnderTest.createMessage(tgtAddress = 0x02.toByte(), msgType = 0x01.toByte(), payload = byteArrayOf(), isBroadcast = false)
        val msgBytes = msg.toByteArray()
        val crc = msg.crc;
        
        // Check the header, values are constants in the companion class of the unit under test
        assertTrue(msgBytes[CommsProtocol.HEADER_POS_0] == CommsProtocol.HEADER_0, "msgBytes[0] should be HEADER_0")
        assertTrue(msgBytes[CommsProtocol.HEADER_POS_1] == CommsProtocol.HEADER_1, "msgBytes[1] should be HEADER_1")
        assertTrue(msgBytes[CommsProtocol.HEADER_POS_2] == CommsProtocol.HEADER_2, "msgBytes[2] should be HEADER_2")
        assertTrue(msgBytes[CommsProtocol.SRC_ADDRESS_POS] == classUnderTest.myAddr, "msgBytes[3] should be myAddr")
        assertTrue(msgBytes[CommsProtocol.TGT_ADDRESS_POS] == 0x02.toByte(), "msgBytes[4] should be 0x02")
        assertTrue(msgBytes[CommsProtocol.MSG_TYPE_POS] == 0x01.toByte(), "msgBytes[5] should be 0x01")
        assertTrue(msgBytes[CommsProtocol.PAYLOAD_LENGTH_MSB_POS] == 0x00.toByte(), "msgBytes[6] should be 0x00")
        assertTrue(msgBytes[CommsProtocol.PAYLOAD_LENGTH_LSB_POS] == 0x00.toByte(), "msgBytes[7] should be 0x00")
        assertTrue(msgBytes[CommsProtocol.PAYLOAD_POS] == (crc.toInt() shr 8).toByte(), "msgBytes[8] should be crc rsh 8")
        assertTrue(msgBytes[CommsProtocol.PAYLOAD_POS + 1] == (crc.toInt() and 0xFF).toByte(), "msgBytes[9] should be crc and 0xFF")
    }

    // Test that a basic message is correctly parsed
    @Test fun testParseMessage() {
        val classUnderTest = CommsProtocol()
        classUnderTest.myAddr = 0x02.toByte()
        val msg = classUnderTest.createMessage(tgtAddress = 0x02.toByte(), msgType = 0x01.toByte(), payload = byteArrayOf(0x01.toByte(), 0x02.toByte()), isBroadcast = false)
        val msgBytes = msg.toByteArray()
        val parsedMsg = classUnderTest.parseMessage(msgBytes)

        // Check the parsed message list size, it should be 1 message long
        assertTrue(parsedMsg.size == 1, "parsedMsg should be 1 message long")
        
        // Get the message object from the list
        val parsedMsgObj = parsedMsg[0]

        // Check the message object
        assertTrue(parsedMsgObj.srcAddress == classUnderTest.myAddr, "parsedMsgObj.srcAddress should be myAddr")
        assertTrue(parsedMsgObj.tgtAddress == 0x02.toByte(), "parsedMsgObj.tgtAddress should be 0x02")
        assertTrue(parsedMsgObj.msgType == 0x01.toByte(), "parsedMsgObj.msgType should be 0x01")
        assertTrue(parsedMsgObj.payload.size == 2, "parsedMsgObj.payload should be 2 bytes long")
        assertTrue(parsedMsgObj.payload[0] == 0x01.toByte(), "parsedMsgObj.payload[0] should be 0x01")
        assertTrue(parsedMsgObj.payload[1] == 0x02.toByte(), "parsedMsgObj.payload[1] should be 0x02")
    }

    // Test that multiple basic messages are correctly parsed
    @Test fun testParseMultipleMessages() {
        val classUnderTest = CommsProtocol()
        classUnderTest.myAddr = 0x02.toByte()
        val msg = classUnderTest.createMessage(tgtAddress = 0x02.toByte(), msgType = 0x01.toByte(), payload = byteArrayOf(0x01.toByte(), 0x02.toByte()), isBroadcast = false)
        val msg2 = classUnderTest.createMessage(tgtAddress = 0x02.toByte(), msgType = 0x01.toByte(), payload = byteArrayOf(0x01.toByte(), 0x02.toByte()), isBroadcast = false)
        val msgBytes = msg.toByteArray() + msg2.toByteArray()
        val parsedMsg = classUnderTest.parseMessage(msgBytes)

        // Check the parsed message list size, it should be 2 messages long
        assertTrue(parsedMsg.size == 2, "parsedMsg should be 2 messages long")

        // Get the message objects from the list
        val parsedMsgObj = parsedMsg[0]
        val parsedMsgObj2 = parsedMsg[1]

        // Check the first message object
        assertTrue(parsedMsgObj.srcAddress == classUnderTest.myAddr, "parsedMsgObj.srcAddress should be myAddr")
        assertTrue(parsedMsgObj.tgtAddress == 0x02.toByte(), "parsedMsgObj.tgtAddress should be 0x02")
        assertTrue(parsedMsgObj.msgType == 0x01.toByte(), "parsedMsgObj.msgType should be 0x01")
        assertTrue(parsedMsgObj.payload.size == 2, "parsedMsgObj.payload should be 2 bytes long")
        assertTrue(parsedMsgObj.payload[0] == 0x01.toByte(), "parsedMsgObj.payload[0] should be 0x01")
        assertTrue(parsedMsgObj.payload[1] == 0x02.toByte(), "parsedMsgObj.payload[1] should be 0x02")

        // Check the second message object
        assertTrue(parsedMsgObj2.srcAddress == classUnderTest.myAddr, "parsedMsgObj2.srcAddress should be myAddr")
        assertTrue(parsedMsgObj2.tgtAddress == 0x02.toByte(), "parsedMsgObj2.tgtAddress should be 0x02")
        assertTrue(parsedMsgObj2.msgType == 0x01.toByte(), "parsedMsgObj2.msgType should be 0x01")
        assertTrue(parsedMsgObj2.payload.size == 2, "parsedMsgObj2.payload should be 2 bytes long")
        assertTrue(parsedMsgObj2.payload[0] == 0x01.toByte(), "parsedMsgObj2.payload[0] should be 0x01")
        assertTrue(parsedMsgObj2.payload[1] == 0x02.toByte(), "parsedMsgObj2.payload[1] should be 0x02")
    }

    // Test that a basic message from a sender and broadcast message is correctly parsed
    @Test fun testParseMessageFromSenderAndBroadcast() {
        val classUnderTest = CommsProtocol()
        classUnderTest.myAddr = 0x02.toByte()
        val msg = classUnderTest.createMessage(tgtAddress = 0x02.toByte(), msgType = 0x01.toByte(), payload = byteArrayOf(0x01.toByte(), 0x02.toByte()), isBroadcast = false)
        val msg2 = classUnderTest.createMessage(tgtAddress = classUnderTest.broadcastAddr, msgType = 0x01.toByte(), payload = byteArrayOf(0x01.toByte(), 0x02.toByte()), isBroadcast = true)
        val msgBytes = msg.toByteArray() + msg2.toByteArray()
        val parsedMsg = classUnderTest.parseMessage(msgBytes)

        // Check the parsed message list size, it should be 2 messages long
        assertTrue(parsedMsg.size == 2, "parsedMsg should be 2 messages long")

        // Get the message objects from the list
        val parsedMsgObj = parsedMsg[0]
        val parsedMsgObj2 = parsedMsg[1]

        // Check the first message object
        assertTrue(parsedMsgObj.srcAddress == classUnderTest.myAddr, "parsedMsgObj.srcAddress should be myAddr")
        assertTrue(parsedMsgObj.tgtAddress == 0x02.toByte(), "parsedMsgObj.tgtAddress should be 0x02")
        assertTrue(parsedMsgObj.msgType == 0x01.toByte(), "parsedMsgObj.msgType should be 0x01")
        assertTrue(parsedMsgObj.payload.size == 2, "parsedMsgObj.payload should be 2 bytes long")
        assertTrue(parsedMsgObj.payload[0] == 0x01.toByte(), "parsedMsgObj.payload[0] should be 0x01")
        assertTrue(parsedMsgObj.payload[1] == 0x02.toByte(), "parsedMsgObj.payload[1] should be 0x02")

        // Check the second message object
        assertTrue(parsedMsgObj2.srcAddress == classUnderTest.myAddr, "parsedMsgObj2.srcAddress should be myAddr")
        assertTrue(parsedMsgObj2.tgtAddress == classUnderTest.broadcastAddr, "parsedMsgObj2.tgtAddress should be broadcastAddr")
        assertTrue(parsedMsgObj2.msgType == 0x01.toByte(), "parsedMsgObj2.msgType should be 0x01")
        assertTrue(parsedMsgObj2.payload.size == 2, "parsedMsgObj2.payload should be 2 bytes long")
        assertTrue(parsedMsgObj2.payload[0] == 0x01.toByte(), "parsedMsgObj2.payload[0] should be 0x01")
        assertTrue(parsedMsgObj2.payload[1] == 0x02.toByte(), "parsedMsgObj2.payload[1] should be 0x02")
    }

    // Test that a basic message can be parsed halfway and then the rest of the message can be parsed
    @Test fun testParseMessageHalfway() {
        val classUnderTest = CommsProtocol()
        classUnderTest.myAddr = 0x02.toByte()
        val msg = classUnderTest.createMessage(tgtAddress = 0x02.toByte(), msgType = 0x01.toByte(), payload = byteArrayOf(0x01.toByte(), 0x02.toByte()), isBroadcast = false)
        val msgBytes = msg.toByteArray()
        val parsedMsg = classUnderTest.parseMessage(msgBytes.sliceArray(0..(msgBytes.size / 2)))
        
        // Check the parsed message list size, it should be 0 messages long
        assertTrue(parsedMsg.size == 0, "parsedMsg should be 0 messages long")

        // Parse the rest of the message
        val parsedMsg2 = classUnderTest.parseMessage(msgBytes.sliceArray(((msgBytes.size / 2) + 1)..(msgBytes.size - 1)))

        // Check the parsed message list size, it should be 1 messages long
        assertTrue(parsedMsg2.size == 1, "parsedMsg2 should be 1 messages long")

        // Get the message object from the list
        val parsedMsgObj = parsedMsg2[0]

        // Check the message object
        assertTrue(parsedMsgObj.srcAddress == classUnderTest.myAddr, "parsedMsgObj.srcAddress should be myAddr")
        assertTrue(parsedMsgObj.tgtAddress == 0x02.toByte(), "parsedMsgObj.tgtAddress should be 0x02")
        assertTrue(parsedMsgObj.msgType == 0x01.toByte(), "parsedMsgObj.msgType should be 0x01")
        assertTrue(parsedMsgObj.payload.size == 2, "parsedMsgObj.payload should be 2 bytes long")
        assertTrue(parsedMsgObj.payload[0] == 0x01.toByte(), "parsedMsgObj.payload[0] should be 0x01")
        assertTrue(parsedMsgObj.payload[1] == 0x02.toByte(), "parsedMsgObj.payload[1] should be 0x02")
    }

    // Test that a basic message that is not for me is parsed but not returned
    @Test fun testParseMessageNotForMe() {
        val classUnderTest = CommsProtocol()
        classUnderTest.myAddr = 0x02.toByte()
        val msg = classUnderTest.createMessage(tgtAddress = 0x01.toByte(), msgType = 0x01.toByte(), payload = byteArrayOf(0x01.toByte(), 0x02.toByte()), isBroadcast = false)
        val msgBytes = msg.toByteArray()
        val parsedMsg = classUnderTest.parseMessage(msgBytes)

        // Check the parsed message list size, it should be 0 messages long
        assertTrue(parsedMsg.size == 0, "parsedMsg should be 0 messages long")
    }
}

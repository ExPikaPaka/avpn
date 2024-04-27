package com.expikapaka.avpn.util
import java.nio.charset.StandardCharsets

///////////// ABOUT THIS PACKAGE /////////////////////////////////////
//  This package contains several functions for debugging and
//    analyzing raw packet data on the Network Layer.
//
//  The purpose is to display headers data (currently IPv4 and IPv6
//    are supported), and packet payload.
//
//
//  USAGE:
//   - use 'displayPacketInfo(byteArray)' to print IPv4/IPv6 header.
//   - use 'displayPacketData(byteArray)' to print packet payload.
//
//////////////////////////////////////////////////////////////////////


// Display packet header info (IPv4 and IPv6)
fun displayPacketInfo(packet: ByteArray) {
    // Extract IP version
    val version = (packet.get(0).toInt() shr 4) and 0xF

    if (version == 4) {
        displayIPv4PacketInfo(packet)
    } else if (version == 6) {
        displayIPv6PacketInfo(packet)
    } else {
        println("Wrong protocol version! Only IPv4 and IPv6 are supported.")
    }
}

// Display packet data (payload except header)
fun displayPacketData(packet: ByteArray) {
    // Check the version of the IP protocol
    val version = (packet[0].toInt() shr 4) and 0xF
    val offset = if (version == 4) {
        // For IPv4, the header length is determined by the IHL field (number of 32-bit words)
        val ihl = packet[0].toInt() and 0xF
        ihl * 4 // Each word is 4 bytes long
    } else if (version == 6) {
        40 // IPv6 header length
    } else {
        println("Wrong protocol version!. Only IPv4 and IPv6 are supported.")
        return
    }
    // Display packet data except header
    println("==================")
    val data = packet.copyOfRange(offset, packet.size)
    println("Packet data:")
    println("[" + data.joinToString(separator = ", ") { (it.toInt() and 0xFF).toString() } + "]")

    // Convert packet data to UTF-8 string and display
    println("==================")
    val utf8String = String(data, StandardCharsets.UTF_8)
    println("UTF-8 data:")
    println(utf8String)
}

// Display IPv4 header info
fun displayIPv4PacketInfo(packet: ByteArray) {
    // Extracting relevant fields from the IPv4 header
    val version = (packet.get(0).toInt() shr 4) and 0xF
    val ihl = packet.get(0).toInt() and 0xF
    val dscp = (packet.get(1).toInt() shr 2) and 0x3F
    val ecn = (packet.get(1).toInt()) and 0x3
    val totalLength = combineBytesToShort(packet.get(2), packet.get(3)).toInt() and 0xFFFF
    val ident = combineBytesToShort(packet.get(4), packet.get(5)).toInt() and 0xFFFF
    val flags = (packet.get(6).toInt() shr 5) and 0x3
    val fragOffset = combineBytesToShort(packet.get(6), packet.get(7)).toInt() and 0x1FFF
    val ttl = packet.get(8).toInt() and 0xFF
    val protocol = packet.get(9).toInt() and 0xFF
    val headChecksum = combineBytesToShort(packet.get(10), packet.get(11)).toInt() and 0xFFFF
    val srcAddress = String.format(
        "%d.%d.%d.%d",
        packet.get(12).toInt() and 0xFF,
        packet.get(13).toInt() and 0xFF,
        packet.get(14).toInt() and 0xFF,
        packet.get(15).toInt() and 0xFF
    )
    val dstAddress = String.format(
        "%d.%d.%d.%d",
        packet.get(16).toInt() and 0xFF,
        packet.get(17).toInt() and 0xFF,
        packet.get(18).toInt() and 0xFF,
        packet.get(19).toInt() and 0xFF
    )

    println("Version  : $version")
    println("IHL      : $ihl")
    println("DSCP     : $dscp")
    println("ECN      : $ecn")
    println("Tot leng : $totalLength")
    println("Identif  : $ident")
    println("Flags    : $flags")
    println("Frag off : $fragOffset")
    println("TTL      : $ttl")
    println("Protocol : $protocol")
    println("Hea chec : $headChecksum")
    println("Source   : $srcAddress")
    println("Destinat : $dstAddress")
}

// Display IPv6 header info
fun displayIPv6PacketInfo(packet: ByteArray) {
    // Extracting relevant fields from the IPv6 header
    val version = (packet[0].toInt() shr 4) and 0xF
    val trafficClass = ((packet[0].toInt() and 0x0F) shl 4) or ((packet[1].toInt() and 0xF0) ushr 4)
    val flowLabel = ((packet[1].toInt() and 0x0F) shl 16) or ((packet[2].toInt() and 0xFF) shl 8) or (packet[3].toInt() and 0xFF)
    val payloadLength = combineBytesToShort(packet[4], packet[5]).toInt() and 0xFFFF
    val nextHeader = packet[6].toInt() and 0xFF
    val hopLimit = packet[7].toInt() and 0xFF
    val srcAddress = "%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X".format(
        packet[8].toInt() and 0xFF, packet[9].toInt() and 0xFF, packet[10].toInt() and 0xFF,
        packet[11].toInt() and 0xFF, packet[12].toInt() and 0xFF, packet[13].toInt() and 0xFF,
        packet[14].toInt() and 0xFF, packet[15].toInt() and 0xFF
    )
    val dstAddress = "%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X".format(
        packet[16].toInt() and 0xFF, packet[17].toInt() and 0xFF, packet[18].toInt() and 0xFF,
        packet[19].toInt() and 0xFF, packet[20].toInt() and 0xFF, packet[21].toInt() and 0xFF,
        packet[22].toInt() and 0xFF, packet[23].toInt() and 0xFF
    )

    println("Version        : $version")
    println("Traffic Class  : $trafficClass")
    println("Flow Label     : $flowLabel")
    println("Payload Length : $payloadLength")
    println("Next Header    : $nextHeader")
    println("Hop Limit      : $hopLimit")
    println("Source Address : $srcAddress")
    println("Destination    : $dstAddress")
}

// Combines 2 bytes into single 2-byte value
private fun combineBytesToShort(byte1: Byte, byte2: Byte): Short {
    return (byte1.toInt() shl 8 or (byte2.toInt() and 0xFF)).toShort()
}
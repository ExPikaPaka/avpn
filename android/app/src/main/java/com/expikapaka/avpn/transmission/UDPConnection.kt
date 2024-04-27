package com.expikapaka.avpn.transmission

import java.net.DatagramPacket
import java.net.DatagramSocket
import java.net.InetAddress

///////////// ABOUT THIS PACKAGE /////////////////////////////////////
//  This package contains UDP connection interface implementation
//
//  The purpose is to establish simple UDP connection
//
//  USAGE:
//   1. Configure connection via .setup(...) function. You need to
//     pass proper endpoint IP, port, and maybe timeOut. If timeout
//     is <= 0 than .read() function has infinite timeout.
//
//   2. Use .write(...) or .read() functions to communicate with
//     endpoint.
//
//   3. Use .close() to close connection.
//
//   4. Use .isConnected() to check connection state
//
//////////////////////////////////////////////////////////////////////

class UDPConnection: IConnection {
    private lateinit var serverAddress: InetAddress // IP address
    private var serverPort = 0 // Server port
    val MTU = 1500 // Maximum Transmission Unit
    private var configured = false // Connection state
    private val writeSync = Object() // Write synchronization
    private val readSync = Object() // Read synchronization
    val socket = DatagramSocket()

    // Configure socket with IP, port, timeout
    override fun setup(ipAddress: String, port: Int, timeOut: Int) {
        if (timeOut > 0) {
            socket.soTimeout = timeOut
        }
        serverAddress = InetAddress.getByName(ipAddress)
        serverPort = port
        configured = true
    }

    // Read function. Returns data
    override fun read(size: Int): ByteArray {
        synchronized(readSync) {
            val buffer = ByteArray(size)
            val packet = DatagramPacket(buffer, buffer.size, serverAddress, serverPort)
            if(configured) {
                socket.receive(packet)
            }

            // Return only the portion of the buffer that contains the received data
            return packet.data.copyOf(packet.length)
        }
    }

    // Write function. Returns true if successful, false otherwise
    override fun write(data: ByteArray): Boolean {
        synchronized(writeSync) {
            if (data.size > MTU) {
                println("Data exceeds MTU. Cannot send.")
                return false
            }
            if(!configured) return false

            val packet = DatagramPacket(data, data.size, serverAddress, serverPort)
            socket.send(packet)
            return true
        }
    }

    // Return connection state
    override fun isConfigured(): Boolean {
        return configured
    }

    // Close the connection
    override fun close() {
        socket.close()
        configured = false
    }
}
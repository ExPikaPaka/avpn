package com.expikapaka.avpn

import android.os.Handler
import android.os.Looper
import java.net.DatagramPacket
import java.net.DatagramSocket
import java.net.InetAddress

class VPNConnection {
    private val timeOut = 3000 // milliseconds
    private val maxPacketSize = 2048

    // Connect to server
    fun connectToServer(ip: String, port: Int, callback: ConnectionCallback) {
        val handler = Handler(Looper.getMainLooper())

        // Create a new thread for network operations
        Thread {
            try {
                // UDP Socket
                val socket = DatagramSocket()
                socket.soTimeout = timeOut

                // Retrieving server IPv4 address
                val servAddress = InetAddress.getByName(ip)

                /////////// Here must be Diffie-Hellman handshake implemented via class ////////////
                // 1. Server sends prime number and primitive root (p,g). We must .receive() them
                // 2. We generate our public and private key
                // 3. We receive server's public key
                // 4. We send to server our public key
                // 5. We calculate our secret shared key.
                ////////////////////////////////////////////////////////////////////////////////////


                // For simplicity for now we just send data and wait "200 OK" from server.
                // Send "Hello"
                val msg = "Hello".toByteArray()
                val sendPacket = DatagramPacket(msg, msg.size, servAddress, port)
                socket.send(sendPacket)

                // Receive response
                val receiveData = ByteArray(maxPacketSize)
                val receivePacket = DatagramPacket(receiveData, receiveData.size)
                socket.receive(receivePacket)
                val response = String(receivePacket.data, 0, receivePacket.length)

                // Check for "ok" response
                if (response.trim() != "200 OK") {
                    // Notify UI about failed connection
                    handler.post {
                        callback.onConnectionResult(false)
                    }
                    return@Thread
                }

                // Close the socket
                socket.close()

                // Notify UI about successful connection
                handler.post {
                    callback.onConnectionResult(true)
                }

            } catch (e: Exception) {
                e.printStackTrace()

                // Notify UI about connection failure
                handler.post {
                    callback.onConnectionResult(false)
                }
            }
        }.start()
    }

    // Callback interface for connection result
    interface ConnectionCallback {
        fun onConnectionResult(success: Boolean)
    }
}

package com.expikapaka.avpn

import android.os.Handler
import android.os.Looper
import com.expikapaka.avpn.transmission.UDPConnection


import kotlin.math.log
///////////// ABOUT THIS PACKAGE /////////////////////////////////////
//  This package contains VPNConnection class that establishes
//    connection between Client (us) and Server.
//
//  Currently it just sends fields like 'login' + 'password' and if
//    server response is 'OK' - than connection is good.
//
//
//////////////////////////////////////////////////////////////////////


// This class makes handshake between Client (us) and Server over UDP socket
class VPNConnection {
    private val timeOut = 3000 // Connection timeout in milliseconds
    // Connect to server. You need to pass server ip, port, and user login, password
    fun connectToServer(ip: String, port: Int, login: String, password: String, callback: ConnectionCallback) {
        val handler = Handler(Looper.getMainLooper())
        val connection = UDPConnection()  // UDP connection

        // Create a new thread for network operations
        Thread {
            try {
                // Setup connection
                connection.setup(ip, port, timeOut)


                /////////// Here must be Diffie-Hellman handshake implemented via class ////////////
                // 1. Server sends prime number and primitive root (p,g). We must .receive() them
                // 2. We generate our public and private key
                // 3. We receive server's public key
                // 4. We send to server our public key
                // 5. We calculate our secret shared key.
                ////////////////////////////////////////////////////////////////////////////////////


                // For simplicity for now we just send data and wait "OK" from server.
                // Send 'login password'
                val msg = "$login $password".toByteArray()
                connection.write(msg)

                // Receive response
                val response = String(connection.read(1500))

                // Check for "OK" response
                if (response == "OK") { // Connection good
                    // Notify UI about successful connection
                    handler.post {
                        callback.onConnectionResult(true)
                    }

                } else { // Connection failed
                    // Notify UI about connection failure
                    handler.post {
                        callback.onConnectionResult(false)
                    }
                }
            } catch (e: Exception) {
                e.printStackTrace() // Print exception

                // Notify UI about connection failure
                handler.post {
                    callback.onConnectionResult(false)
                }
            }
            connection.close()
        }.start()
    }

    // Callback interface for connection result
    interface ConnectionCallback {
        fun onConnectionResult(success: Boolean)
    }
}

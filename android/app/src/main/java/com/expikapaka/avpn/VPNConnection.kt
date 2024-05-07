package com.expikapaka.avpn

import android.os.Handler
import android.os.Looper
import com.expikapaka.avpn.encryption.AES128
import com.expikapaka.avpn.encryption.DiffieHellman
import com.expikapaka.avpn.encryption.SHA256
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


// TODO:
//   - Remove debug code.
//


// This class makes handshake between Client (us) and Server over UDP socket
class VPNConnection {
    private val timeOut = 10000 // Connection timeout in milliseconds
    // Connect to server. You need to pass server ip, port, and user login, password
    fun connectToServer(ip: String, port: Int, login: String, password: String, callback: ConnectionCallback) {
        val handler = Handler(Looper.getMainLooper())
        val connection = UDPConnection()  // UDP connection

        // Create a new thread for network operations
        Thread {
            try {
                // Setup connection
                connection.setup(ip, port, timeOut)

                // Generating Diffie-Hellman values
                val diffi = DiffieHellman()
                val p = diffi.generatePrime(32)
                val g = diffi.generatePrimitiveRoot(p)
                val privateKey = diffi.generatePrivateKey(p)
                val publicKey = diffi.generatePublicKey(p, g, privateKey)
                val diffiString = "$p $g $publicKey"
                println("P              :" + p)
                println("G              :" + g)
                println("privateKey     :" + privateKey)
                println("publicKey      :" + publicKey)
                println("diffiString    :" + diffiString)

                // Send parameters to the server
                connection.write(diffiString.toByteArray())

                // Receive response
                var response = String(connection.read(1500))

                // Split the response string into 'sharedSecret' and 'localIP' using space as delimiter
                var parts = response.split(" ")

                // Extract 'sharedSecret' and 'localIP' from the parts
                val sharedSecretString = parts[0]
                val localIPString = parts[1]

                println("response       :" + response)
                val sharedSecret = diffi.calculateSharedSecret(sharedSecretString.toULong(), privateKey, p)
                println("sharedSecret   :" + sharedSecret)

                // Calculating key via SHA256
                val sha = SHA256()
                val key = sha.getSHA64(sharedSecret.toString())

                // Encrypting authentication fields
                val aes = AES128()
                val authString = "$login $password"
                val authEncrypted = aes.encrypt(authString.toByteArray(), key.toByteArray())
                println("key            :" + key)
                println("authEncrypted  :" + authEncrypted)

                // Send encrypted auth data to the server
                connection.write(authEncrypted)

                // Receive server response
                response = String(connection.read(1500))
                println("response       :" + response)
                parts = response.split(" ")

                // Extract 'OK' and 'port'
                val okMessage = parts[0]
                val vpnServicePort = parts[1].toInt()

                // Check for "OK" response
                if (okMessage == "OK") { // Connection good
                    // Notify UI about successful connection
                    handler.post {
                        callback.onConnectionResult(true, key, localIPString, vpnServicePort)
                    }

                } else { // Connection failed
                    // Notify UI about connection failure
                    handler.post {
                        callback.onConnectionResult(false, null, null, null)
                    }
                }
            } catch (e: Exception) {
                e.printStackTrace() // Print exception

                // Notify UI about connection failure
                handler.post {
                    callback.onConnectionResult(false, null, null, null)
                }
            }
            connection.close()
        }.start()
    }

    // Callback interface for connection result
    interface ConnectionCallback {
        fun onConnectionResult(success: Boolean, sharedSecret: String?, localIP: String?, serverPort: Int?)
    }
}

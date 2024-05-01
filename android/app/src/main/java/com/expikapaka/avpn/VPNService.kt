package com.expikapaka.avpn

import android.content.Intent
import android.net.VpnService
import android.os.ParcelFileDescriptor
import com.expikapaka.avpn.encryption.AES128
import java.io.FileInputStream
import java.io.FileOutputStream
import java.io.IOException
import com.expikapaka.avpn.transmission.IConnection
import com.expikapaka.avpn.transmission.UDPConnection
import com.expikapaka.avpn.util.displayPacketData
import com.expikapaka.avpn.util.displayPacketInfo
import kotlin.concurrent.thread


///////////// ABOUT THIS PACKAGE /////////////////////////////////////
//  This package contains VPNService class which purpose is to
//    redirect all the traffic from Android device to the Server
//
//////////////////////////////////////////////////////////////////////


// TODO:
//   - Remove hardcoded server IP and port. Use 'Intent' to retrieve those fields
//      from caller.
//   - Remove debug code.
//
//


// VPN service class that redirects all the traffic to the server
class VPNService : VpnService() {
    private lateinit var vpnThread: Thread // Service thread. Used to separate from UI thread
    private lateinit var parcelFileDescriptor: ParcelFileDescriptor // 'TUN' device
    private lateinit var connection: UDPConnection // UDP connection to the server
    private val ACTION_START = "start"       // VPN Service 'start' action
    private val ACTION_STOP = "stop"         // VPN Service 'stop' action
    private val cloc = Object()              // Console synchronization
    private var txCnt = 0                    // Transmit packet count
    private var rxCnt = 0                    // Receive packet count
    private val maxPacketSize = 2400         // MTU
    private var timeOut: Long = 60000        // 60 seconds. If after last TX packet we have no RX - then disconnect
    private val routeIP = "0.0.0.0"          // Route address
    private val routeMask = 0                // Route mask
    private var serverIP = ""                // VPN server address
    private var serverPort = 1234            // VPN server port
    private var sharedSecret = ""            // Encryption string
    private var localIP = ""                 // This device private network address
    private var localMask = 24               // This device private network mask
    private val DEFAULT_SERVER_PORT = 25565  // Default server port
    private val DEFAULT_LOCAL_MASK = 24      // Default server port
    private var lastTimeResetTimer: Long = 0 // Timer value to stop service in case timeout
    private val aes = AES128()               // AES128 encryptor/decryptor

    // Create function
    override fun onCreate() {
        super.onCreate()
        println("Service creation.")
    }

    // Start function. Executes each time when startService() is called.
    // Depending on Intent field 'action' starts or stops service.
    override fun onStartCommand(intent: Intent?, flags: Int, startId: Int): Int {
        if(intent != null) {
            if(intent.action == ACTION_START) {
                // Retrieve fields from Intent
                val serverIP = intent.getStringExtra("serverIP")
                val serverPort = intent.getIntExtra("serverPort", DEFAULT_SERVER_PORT)
                val localIP = intent.getStringExtra("localIP")
                val localMask = intent.getIntExtra("localMask", DEFAULT_LOCAL_MASK)
                val sharedSecret = intent.getStringExtra("sharedSecret")

                // Check if serverIP and serverPort are not null
                if(serverIP != null && localIP != null && sharedSecret != null) {
                    this.serverIP = serverIP
                    this.serverPort = serverPort
                    this.localIP = localIP
                    this.localMask = localMask
                    this.sharedSecret = sharedSecret
                    startVPN()
                } else {
                    stopVPN()
                }
            } else if(intent.action == ACTION_STOP) {
                stopVPN() // Stop service
            }
         }
        return START_STICKY
    }

    // Start redirecting traffic. Basically configures VPN and starts read/write
    //  functions in separate threads.
    private fun startVPN() {
        vpnThread = Thread {
            try {
                // Create a UDPConnection instance
                connection = UDPConnection()

                // Initialize connection
                connection.setup(serverIP, serverPort, 0)

                // Protect socket from loopback
                protect(connection.socket)

                // Create a new VPN Builder
                val builder = Builder()

                // Set the VPN parameters. Later you need to get that information from the server
                builder.setSession(getString(R.string.app_name))
                    .addAddress(localIP, localMask)
                    .addRoute(routeIP, routeMask)
                    .setMtu(maxPacketSize)

                // Establish the VPN connection
                parcelFileDescriptor = builder.establish()!!

                // Redirect network traffic through the VPN interface
                val vpnInput = FileInputStream(parcelFileDescriptor.fileDescriptor)
                val vpnOutput = FileOutputStream(parcelFileDescriptor.fileDescriptor)

                // Updating timer
                lastTimeResetTimer = System.currentTimeMillis()

                // Run client to server communication
                thread (start = true) {
                   runClientToServer(vpnInput, connection)
                }

                // Run Server to client communication
                thread (start = true) {
                    runServerToClient(vpnOutput, connection)
                }

            } catch (e: Exception) {
                // Handle VPN connection errors
                e.printStackTrace()
            } finally {

            }
        }

        vpnThread.start()
    }

    // Runs infinite loop communication from Client to Server
    private fun runClientToServer(vpnInputStream: FileInputStream, serverConnection: IConnection) {
        while (true) {
            // Leave the loop if the socket is closed
            try {
                clientToServer(vpnInputStream, serverConnection)
            }
            catch (e: Exception) {
                stopVPN()
                break
            }
        }
    }

    // Reads local TUN device and sends packet to server via connection
    private fun clientToServer(vpnInputStream: FileInputStream, serverConnection: IConnection) {
        // Check timeout
        val currentTime = System.currentTimeMillis()
        if (currentTime - lastTimeResetTimer > timeOut) {
            stopVPN()
        }

        // Read form Us and send to Server
        var readBuffer = ByteArray(maxPacketSize) // Packet buffer
        // Read from our device
        val numberOfBytesReceived = vpnInputStream.read(readBuffer)

        if (numberOfBytesReceived <= 0) {
            return
        }

        // Reducing buffer size to exact amount which we read
        readBuffer = readBuffer.dropLast(readBuffer.size - numberOfBytesReceived).toByteArray()

        // Output (DEBUG)
        //synchronized(cloc) {
        //    println("\n\n\nTX: $txCnt\nRead packet: " + numberOfBytesReceived + " Bytes\n")
        //    displayPacketInfo(readBuffer)
        //    displayPacketData(readBuffer)
        //}

        // Encrypting
        val encrypted = aes.encrypt(readBuffer, sharedSecret.toByteArray())

        // Sending packet
        serverConnection.write(encrypted)
        txCnt++ // Increment transmit count
    }

    // Runs infinite loop communication from Server to Client
    private fun runServerToClient(vpnOutputStream: FileOutputStream, serverConnection: IConnection) {
        while(true) {
            // Leave loop if the socket is closed
            try {
                serverToClient(vpnOutputStream, serverConnection)
            }
            catch (e: Exception) {
                stopVPN()
                break
            }
        }
    }

    // Receive packets from server and write to local TUN device
    private fun serverToClient(vpnOutputStream: FileOutputStream, serverConnection: IConnection) {
        val messageData = serverConnection.read(maxPacketSize)
        lastTimeResetTimer = System.currentTimeMillis() // Reset timer
        rxCnt++ // Increment receive count

        // Output (DEBUG)
        //synchronized(cloc) {
        //    println("\n\nRX: $rxCnt\nGot packet from server:\n")
        //    displayPacketInfo(messageData)
        //    displayPacketData(messageData)
        //}

        // Decrypting
        val decrypted = aes.decrypt(messageData, sharedSecret.toByteArray())
        vpnOutputStream.write(decrypted)
    }

    // Stop service via closing connection
    private fun stopVPN() {
        println("Service stop")
        // Just close every connection and stopSelf
        //
        // I use try-catch exception to stop service
        //
        // After socket and file descriptor close the exception will raise
        //   inside the main loop which will lead to thread stop.
        //
        //
        // Is it a good approach? I don't know. I just found this solution
        //   at 'Operator Foundation' github, and it works (I tested it)
        //
        // Link: https://github.com/OperatorFoundation/MoonbounceAndroidKotlin/
        // blob/main/moonbounceVPNService/src/main/java/org/operatorfoundation/
        // moonbouncevpnservice/MBAKVpnService.kt


        try {
            // Close VPN file descriptor
            parcelFileDescriptor.close()
        } catch (e: Exception) {
            e.printStackTrace()
        }
        try {
            // Close connection
            connection.close()
        } catch (e: Exception) {
            e.printStackTrace()
        }
        try {
            // Stop service. (onDestroy() call?)
            stopSelf()
        } catch (e: Exception) {
            e.printStackTrace()
        }
    }
}
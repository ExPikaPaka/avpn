package com.expikapaka.avpn

import android.content.Intent
import android.net.VpnService
import android.os.ParcelFileDescriptor
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
//   - When TUN on server will be good, check packet data and internet connection.
//


// VPN service class that redirects all the traffic to the server
class VPNService : VpnService() {
    private lateinit var vpnThread: Thread // Service thread. Used to separate from UI thread
    private lateinit var parcelFileDescriptor: ParcelFileDescriptor // 'TUN' device

    private val maxPacketSize = 1500 // MTU

    private val ACTION_START = "start" // VPN Service 'start' action
    private val ACTION_STOP = "stop" // VPN Service 'stop' action

    private val cloc = Object() // Console synchronization

    private var txCnt = 0 // Transmit packet count
    private var rxCnt = 0 // Receive packet count

    private lateinit var connection: UDPConnection

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
                startVPN() // Running connection
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
                // Hardcoded IP server. I setup static IP for my server on the home router
                val serverAddress = "192.168.0.120"
                val serverPort = 25565

                // Create a UDPConnection instance
                connection = UDPConnection()

                // Initialize connection
                connection.setup(serverAddress, serverPort, 0)

                // Protect socket from loopback
                println("Is protected????: " + protect(connection.socket))

                // Create a new VPN Builder
                val builder = Builder()

                // Set the VPN parameters. Later you need to get that information from the server
                builder.setSession(getString(R.string.app_name))
                    .addAddress("10.0.0.2", 24)
                    .addRoute("0.0.0.0", 0)
                    .setMtu(maxPacketSize)

                // Establish the VPN connection
                parcelFileDescriptor = builder.establish()!!

                // Redirect network traffic through the VPN interface
                val vpnInput = FileInputStream(parcelFileDescriptor.fileDescriptor)
                val vpnOutput = FileOutputStream(parcelFileDescriptor.fileDescriptor)


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
        var readBuffer = ByteArray(maxPacketSize)

        val numberOfBytesReceived = vpnInputStream.read(readBuffer)

        if (numberOfBytesReceived == -1) {
            throw IOException()
        }
        if (numberOfBytesReceived == 0) {
            return
        }

        // Reducing buffer size to exact amount which we read
        readBuffer = readBuffer.dropLast(readBuffer.size - numberOfBytesReceived).toByteArray()

        // Output (DEBUG)
        synchronized(cloc) {
            println("\n\n\nTX: $txCnt\nRead packet: " + numberOfBytesReceived + " Bytes\n")
            displayPacketInfo(readBuffer)
            displayPacketData(readBuffer)
        }

        // Sending packet
        serverConnection.write(readBuffer)
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
        rxCnt++ // Increment receive count

        // Output (DEBUG)
        synchronized(cloc) {
            println("\n\nRX: $rxCnt\nGot packet from server:\n")
            displayPacketInfo(messageData)
            displayPacketData(messageData)
        }
        vpnOutputStream.write(messageData)
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
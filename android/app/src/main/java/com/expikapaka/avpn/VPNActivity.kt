package com.expikapaka.avpn

import android.content.Intent
import android.net.VpnService
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.widget.Button
import android.widget.EditText
import android.widget.TextView
import com.expikapaka.avpn.encryption.AES128


///////////// ABOUT THIS PACKAGE /////////////////////////////////////
//  This package contains main activity. It creates user interface
//    and other interactions.
//
//  In the context of C/C++. This file contains 'main' entry point.
//
//////////////////////////////////////////////////////////////////////


// TODO:
//   - Remove two buttons 'connect' and 'disconnect - use only one
//       as a switch.
//   - Create a better looking UI.
//   - Remove hardcoded text of localIP and localMask. Retrieve
//       those fields from server. (line 146)
//


// Activity class (UI + interaction). Basically that's our main app
class VPNActivity : AppCompatActivity(), VPNConnection.ConnectionCallback  {
    // Some fields that program uses for his logic
    private val VPN_PREPARE_REQUEST = 1
    private var isConnected = false
    private lateinit var connectButton: Button
    private lateinit var disconnectButton: Button
    private lateinit var infoTextView: TextView
    private lateinit var connection: VPNConnection
    private lateinit var ipEditText: EditText
    private lateinit var portEditText: EditText
    private lateinit var loginEditText: EditText
    private lateinit var passwordEditText: EditText
    private var isVpnServiceRunning = false

    // Function that executes only once, when app starts.
    // It properly creates app.
    override fun onCreate(savedInstanceState: Bundle?) {
        // Initialization...
        super.onCreate(savedInstanceState)
        setContentView(R.layout.vpn_activity_layout)

        // Retrieve UI elements
        connectButton = findViewById<Button>(R.id.connectButton)
        disconnectButton = findViewById<Button>(R.id.disconnectButton)
        infoTextView = findViewById<TextView>(R.id.infoTextView)
        ipEditText = findViewById<EditText>(R.id.ipEditText)
        portEditText = findViewById<EditText>(R.id.portEditText)
        loginEditText = findViewById<EditText>(R.id.loginEditText)
        passwordEditText = findViewById<EditText>(R.id.passwordEditText)

        // Preparing VPN service
        prepareVpnService()
        connection = VPNConnection()


        // Connect button listener
        connectButton.setOnClickListener {
            connectToServer()
        }

        // Disconnect button listener
        disconnectButton.setOnClickListener {
            stopVPNService()
        }
    }



    // 'Handshake' function between Client and Server. Establish connection.
    private fun connectToServer() {
        try {
            // Retrieving fields
            val serverIP = ipEditText.text.toString()
            val serverPort = portEditText.text.toString().toInt()
            val login = loginEditText.text.toString()
            val password = passwordEditText.text.toString()

            // Connecting
            connection.connectToServer(serverIP, serverPort, login, password, this)

            // Updating user Info field
            updatePacketInfo("Connecting...")
        } catch (e: Exception) {
            // Notify user about error
            updatePacketInfo("Failed to connect!")

            // Print exception for debug purpose
            e.printStackTrace()
        }
    }

    // Start VPN service that redirects all the traffic to the VPN server
    private fun startVPNService(serverIP: String, serverPort: Int, localIP: String, localMask: Int, sharedSecret: String) {
        if (!isVpnServiceRunning) {
            // Starting VPN service
            val startIntent = Intent(this, VPNService::class.java)
            startIntent.action = "start"
            startIntent.putExtra("serverIP", serverIP)
            startIntent.putExtra("serverPort", serverPort)
            startIntent.putExtra("localIP", localIP)
            startIntent.putExtra("localMask", localMask)
            startIntent.putExtra("sharedSecret", sharedSecret)
            startService(startIntent)

            isVpnServiceRunning = true

            // Notify user
            updatePacketInfo("VPN Service started.")
        } else {
            // Temporary. Later will use only one button that works like a switch.
            updatePacketInfo("VPN Service is already running.")
        }
    }

    // Stop redirecting traffic
    private fun stopVPNService() {
        if (isVpnServiceRunning) {
            // Stopping service
            val stopIntent = Intent(this, VPNService::class.java)
            stopIntent.action = "stop"
            startService(stopIntent)
            isVpnServiceRunning = false
            updatePacketInfo("VPN Service stopped.")
        } else {
            updatePacketInfo("VPN Service is not running.")
        }
    }

    // Callback which starts VPN service on successful handshake between client and server
    override fun onConnectionResult(success: Boolean, sharedSecret: String?) {
        runOnUiThread {
            isConnected = if (success && sharedSecret != null) {
                updatePacketInfo("Connected!")
                // REMOVE HARDCODED VALUES
                val localIP = "10.0.0.2"
                val localMask = 24
                val serverIP = ipEditText.text.toString()
                val serverPort = portEditText.text.toString().toInt()
                startVPNService(serverIP, serverPort, localIP, localMask, sharedSecret)
                true
            } else {
                updatePacketInfo("Failed to connect!")
                false
            }
        }
    }

    // Updates user 'info' field
    private fun updatePacketInfo(packetInfo: String) {
        infoTextView.text = packetInfo
    }

    // Ask user for VPN rights
    private fun prepareVpnService() {
        try {
            val intent = VpnService.prepare(this)
            if (intent != null) {
                // Ask user for rights
                startActivityForResult(intent, VPN_PREPARE_REQUEST)
            }
        } catch (e: Exception) {
            e.printStackTrace()
        }
    }
}
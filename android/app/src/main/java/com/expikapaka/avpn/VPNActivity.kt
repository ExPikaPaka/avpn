package com.expikapaka.avpn

import android.content.BroadcastReceiver
import android.content.Context
import android.content.Intent
import android.content.IntentFilter
import android.net.VpnService
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.widget.Button
import android.widget.EditText
import android.widget.TextView
import androidx.localbroadcastmanager.content.LocalBroadcastManager
import com.expikapaka.avpn.encryption.AES128


///////////// ABOUT THIS PACKAGE /////////////////////////////////////
//  This package contains main activity. It creates user interface
//    and other interactions.
//
//  In the context of C/C++. This file contains 'main' entry point.
//
//////////////////////////////////////////////////////////////////////


// TODO:
//   - Remove hardcoded text of localIP and localMask. Retrieve
//       those fields from server. (line 146)
//


// Activity class (UI + interaction). Basically that's our main app
class VPNActivity : AppCompatActivity(), VPNConnection.ConnectionCallback {
    // Some fields that program uses for his logic
    private val VPN_PREPARE_REQUEST = 1
    private var isConnected = false
    private lateinit var connectButton: Button
    private lateinit var infoTextView: TextView
    private lateinit var connection: VPNConnection
    private lateinit var ipEditText: EditText
    private lateinit var portEditText: EditText
    private lateinit var loginEditText: EditText
    private lateinit var passwordEditText: EditText
    private var isVpnServiceRunning = false

    private val serviceStateReceiver = object : BroadcastReceiver() {
        override fun onReceive(context: Context?, intent: Intent?) {
            when (intent?.action) {
                "VPN_SERVICE_STARTED" -> onVPNServiceStarted()
                "VPN_SERVICE_STOPPED" -> onVPNServiceStopped()
            }
        }
    }

    // Function that executes only once, when app starts.
    // It properly creates app.
    override fun onCreate(savedInstanceState: Bundle?) {
        // Initialization...
        super.onCreate(savedInstanceState)
        setContentView(R.layout.vpn_activity_layout)

        // Register broadcast receiver
        val filter = IntentFilter().apply {
            addAction("VPN_SERVICE_STARTED")
            addAction("VPN_SERVICE_STOPPED")
        }
        LocalBroadcastManager.getInstance(this).registerReceiver(serviceStateReceiver, filter)


        // Retrieve UI elements
        connectButton = findViewById<Button>(R.id.connectButton)
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
            connectDisconnect()
        }
    }

    private fun connectDisconnect() {
        if (!isVpnServiceRunning) {
            connectToServer()
        } else {
            stopVPNService()
        }
    }

    // 'Handshake' function between Client and Server. Establish connection.
    private fun connectToServer() {
        try {
            if(!isConnected) {
                // Retrieving fields
                val serverIP = ipEditText.text.toString()
                val serverPort = portEditText.text.toString().toInt()
                val login = loginEditText.text.toString()
                val password = passwordEditText.text.toString()

                // Connecting
                connection.connectToServer(serverIP, serverPort, login, password, this)
                isConnected = true

                // Updating user Info field
                updatePacketInfo("Connecting...")
            }
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
        } else {
            // Temporary. Later will use only one button that works like a switch.
            updatePacketInfo("VPN Service is already running")
        }
    }

    // Stop redirecting traffic
    private fun stopVPNService() {
        if (isVpnServiceRunning) {
            // Stopping service
            val stopIntent = Intent(this, VPNService::class.java)
            stopIntent.action = "stop"
            startService(stopIntent)
        } else {
            updatePacketInfo("VPN Service is not running")
        }
    }

    // Callback which starts VPN service on successful handshake between client and server
    override fun onConnectionResult(success: Boolean, sharedSecret: String?, localIP: String?, serverPort: Int?) {
        runOnUiThread {
            isConnected = if (success && sharedSecret != null && localIP != null && serverPort != null) {
                updatePacketInfo("Connected!")
                // REMOVE HARDCODED VALUES
                val localMask = 24
                val serverIP = ipEditText.text.toString()
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

    // VPN service started callback
    private fun onVPNServiceStarted() {
        isVpnServiceRunning = true
        updatePacketInfo("VPN Service started")
        connectButton.text= getString(R.string.disconnect_button)
    }

    // VPN service stopped callback
    private fun onVPNServiceStopped() {
        // Handle UI updates or other actions when the VPN service stops
        isVpnServiceRunning = false
        isConnected = false
        updatePacketInfo("VPN Service stopped")
        connectButton.text= getString(R.string.connect_button)
    }
}
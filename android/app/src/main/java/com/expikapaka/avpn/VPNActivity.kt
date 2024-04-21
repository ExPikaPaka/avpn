package com.expikapaka.avpn

import android.content.Intent
import android.net.VpnService
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.widget.Button
import android.widget.EditText
import android.widget.TextView

class VPNActivity : AppCompatActivity(), VPNConnection.ConnectionCallback  {
    private val VPN_PREPARE_REQUEST = 1
    private var isConnected = false
    private var vpnService: Intent? = null
    private lateinit var connectButton: Button
    private lateinit var disconnectButton: Button
    private lateinit var infoTextView: TextView
    private lateinit var connection: VPNConnection
    private var isVpnServiceRunning = false

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.vpn_activity_layout)

        connectButton = findViewById<Button>(R.id.connectButton)
        disconnectButton = findViewById<Button>(R.id.disconnectButton)
        infoTextView = findViewById<TextView>(R.id.infoTextView)

        // Preparing VPN service
        prepareVpnService()
        connection = VPNConnection()


        // Buttons listener
        connectButton.setOnClickListener {
            connectToServer()
        }
        disconnectButton.setOnClickListener {

        }
    }
    private fun connectToServer() {
        try {
            val ipEditText = findViewById<EditText>(R.id.ipEditText)
            val portEditText = findViewById<EditText>(R.id.portEditText)

            // Retrieving fields
            val serverIP = ipEditText.text.toString()
            val serverPort = portEditText.text.toString().toInt()
            // Connecting
            connection.connectToServer(serverIP, serverPort, this)
            updatePacketInfo("Connecting...")
        } catch (e: Exception) {
            updatePacketInfo("Failed to connect!")
            e.printStackTrace()
        }
    }
    override fun onConnectionResult(success: Boolean) {
        runOnUiThread {
            isConnected = if (success) {
                updatePacketInfo("Connected!")
                true
            } else {
                updatePacketInfo("Failed to connect!")
                false
            }
        }
    }

    private fun updatePacketInfo(packetInfo: String) {
        infoTextView.text = packetInfo
    }
    private fun prepareVpnService() {
        try {
            // Check if VPN is supported
            val intent = VpnService.prepare(this)
            if (intent != null) {
                // Ask user for rights
                startActivityForResult(intent, VPN_PREPARE_REQUEST)
            } else {
                // VPN is already prepared or not required
            }
        } catch (e: Exception) {
            e.printStackTrace()
        }
    }
}
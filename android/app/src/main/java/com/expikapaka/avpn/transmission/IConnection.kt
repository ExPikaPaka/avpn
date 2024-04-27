package com.expikapaka.avpn.transmission

///////////// ABOUT THIS PACKAGE /////////////////////////////////////
//  This package contains connection interface
//
//////////////////////////////////////////////////////////////////////

// Connection interface
interface IConnection {
    // Read data from connection limited by size
    fun read(size: Int): ByteArray

    // Write data to connection
    fun write(data: ByteArray): Boolean

    // Configure connection
    fun setup(ipAddress: String, port: Int, timeOut: Int)

    // Return connection state
    fun isConfigured(): Boolean

    // Close connection
    fun close()
}



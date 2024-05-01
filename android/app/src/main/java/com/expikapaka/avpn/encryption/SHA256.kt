package com.expikapaka.avpn.encryption

import java.lang.StringBuilder
import java.nio.charset.StandardCharsets
import java.util.Arrays


///////////// ABOUT THIS PACKAGE /////////////////////////////////////
//  This package contains class for SHA-256 computation.
//
//  SHA256 Class:
//    - Provides methods for computing SHA-256 hashes of strings
//
//////////////////////////////////////////////////////////////////////



// SHA256 class for computing SHA-256 hash of 32 bytes and 16 bytes
class SHA256 {
    private var mBlocklen = 0
    private var mBitlen: Long = 0
    private val mData = IntArray(64)
    private val mState = IntArray(8)
    private var messageDigest: ByteArray? = null

    // Initialization
    init {
        mState[0] = 0x6a09e667.toInt()
        mState[1] = 0xbb67ae85.toInt()
        mState[2] = 0x3c6ef372.toInt()
        mState[3] = 0xa54ff53a.toInt()
        mState[4] = 0x510e527f.toInt()
        mState[5] = 0x9b05688c.toInt()
        mState[6] = 0x1f83d9ab.toInt()
        mState[7] = 0x5be0cd19.toInt()
    }

    // Function to compute SHA-256 hash of a string (64 characters)
    fun getSHA256(string: String): String {
        update(string)
        return toString32(digest())
    }

    // Function to compute SHA-128 hash of a string (32 characters)
    fun getSHA128(string: String): String {
        update(string)
        return toString32(digest())
    }

    // Function to compute SHA-64 hash of a string (16 characters)
    fun getSHA64(string: String): String {
        update(string)
        return toString16(digest())
    }

    // Update internal state with byte array data
    private fun update(data: ByteArray) {
        for (element in data) {
            mData[mBlocklen++] = element.toInt() and 0xFF
            if (mBlocklen == 64) {
                transform()
                mBitlen += 512
                mBlocklen = 0
            }
        }
    }

    // Update internal state with string data
    private fun update(data: String) {
        update(data.toByteArray(StandardCharsets.UTF_8))
    }

    // Compute final hash value
    private fun digest(): ByteArray {
        pad()
        revert()
        return messageDigest ?: throw NullPointerException("Message digest is null")
    }

    private fun rotr(x: Int, n: Int): Int {
        return x ushr n or (x shl 32 - n)
    }

    private fun choose(e: Int, f: Int, g: Int): Int {
        return e and f xor (e.inv() and g)
    }

    private fun majority(a: Int, b: Int, c: Int): Int {
        return a and b or (a and c) or (b and c)
    }

    private fun sig0(x: Int): Int {
        return rotr(x, 7) xor rotr(x, 18) xor (x ushr 3)
    }

    private fun sig1(x: Int): Int {
        return rotr(x, 17) xor rotr(x, 19) xor (x ushr 10)
    }

    private fun transform() {
        val m = IntArray(64)

        for (i in 0..15) {
            m[i] = (mData[i * 4] shl 24) or (mData[i * 4 + 1] shl 16) or (mData[i * 4 + 2] shl 8) or (mData[i * 4 + 3])
        }

        for (i in 16 until 64) {
            m[i] = sig1(m[i - 2]) + m[i - 7] + sig0(m[i - 15]) + m[i - 16]
        }

        val state = mState.copyOf()

        for (i in 0 until 64) {
            val maj = majority(state[0], state[1], state[2])
            val xorA = rotr(state[0], 2) xor rotr(state[0], 13) xor rotr(state[0], 22)
            val ch = choose(state[4], state[5], state[6])
            val xorE = rotr(state[4], 6) xor rotr(state[4], 11) xor rotr(state[4], 25)
            val sum = m[i] + K[i] + state[7] + ch + xorE
            val newA = xorA + maj + sum
            val newE = state[3] + sum
            state[7] = state[6]
            state[6] = state[5]
            state[5] = state[4]
            state[4] = newE
            state[3] = state[2]
            state[2] = state[1]
            state[1] = state[0]
            state[0] = newA
        }

        for (i in 0 until 8) {
            mState[i] += state[i]
        }
    }

    // Pad message to ensure it is multiple of 512 bits
    private fun pad() {
        val i = mBlocklen
        val end = if (mBlocklen < 56) 56 else 64

        mData[i] = 0x80
        var j = i + 1
        while (j < end) {
            mData[j] = 0x00
            j++
        }

        if (mBlocklen >= 56) {
            transform()
            Arrays.fill(mData, 0, 56, 0)
        }

        mBitlen += mBlocklen.toLong() * 8
        mData[63] = (mBitlen).toInt()
        mData[62] = (mBitlen shr 8).toInt()
        mData[61] = (mBitlen shr 16).toInt()
        mData[60] = (mBitlen shr 24).toInt()
        mData[59] = (mBitlen shr 32).toInt()
        mData[58] = (mBitlen shr 40).toInt()
        mData[57] = (mBitlen shr 48).toInt()
        mData[56] = (mBitlen shr 56).toInt()
        transform()
        messageDigest = ByteArray(32)
        revert()
    }

    // Revert internal state to generate final hash
    private fun revert() {
        val hash = ByteArray(32)
        for (i in 0 until 4) {
            for (j in 0 until 8) {
                hash[i + j * 4] = (mState[j] shr (24 - i * 8)).toByte()
            }
        }
        messageDigest = hash
    }

    // Convert byte array to hex string (32 characters)
    private fun toString64(digest: ByteArray): String {
        val sb = StringBuilder()
        for (i in digest.indices) {
            sb.append(String.format("%02x", digest[i]))
        }
        return sb.toString()
    }

    // Convert byte array to hex string (16 characters)
    private fun toString32(digest: ByteArray): String {
        val sb = StringBuilder()
        for (i in 0 until 16) {
            sb.append(String.format("%02x", digest[i]))
        }
        return sb.toString()
    }

    // Convert byte array to hex string (16 characters)
    private fun toString16(digest: ByteArray): String {
        val sb = StringBuilder()
        for (i in 0 until 8) {
            sb.append(String.format("%02x", digest[i]))
        }
        return sb.toString()
    }

    // Constants used in SHA-256 algorithm
    private val K = intArrayOf(
        0x428a2f98.toInt(), 0x71374491.toInt(), 0xb5c0fbcf.toInt(), 0xe9b5dba5.toInt(),
        0x3956c25b.toInt(), 0x59f111f1.toInt(), 0x923f82a4.toInt(), 0xab1c5ed5.toInt(),
        0xd807aa98.toInt(), 0x12835b01.toInt(), 0x243185be.toInt(), 0x550c7dc3.toInt(),
        0x72be5d74.toInt(), 0x80deb1fe.toInt(), 0x9bdc06a7.toInt(), 0xc19bf174.toInt(),
        0xe49b69c1.toInt(), 0xefbe4786.toInt(), 0x0fc19dc6.toInt(), 0x240ca1cc.toInt(),
        0x2de92c6f.toInt(), 0x4a7484aa.toInt(), 0x5cb0a9dc.toInt(), 0x76f988da.toInt(),
        0x983e5152.toInt(), 0xa831c66d.toInt(), 0xb00327c8.toInt(), 0xbf597fc7.toInt(),
        0xc6e00bf3.toInt(), 0xd5a79147.toInt(), 0x06ca6351.toInt(), 0x14292967.toInt(),
        0x27b70a85.toInt(), 0x2e1b2138.toInt(), 0x4d2c6dfc.toInt(), 0x53380d13.toInt(),
        0x650a7354.toInt(), 0x766a0abb.toInt(), 0x81c2c92e.toInt(), 0x92722c85.toInt(),
        0xa2bfe8a1.toInt(), 0xa81a664b.toInt(), 0xc24b8b70.toInt(), 0xc76c51a3.toInt(),
        0xd192e819.toInt(), 0xd6990624.toInt(), 0xf40e3585.toInt(), 0x106aa070.toInt(),
        0x19a4c116.toInt(), 0x1e376c08.toInt(), 0x2748774c.toInt(), 0x34b0bcb5.toInt(),
        0x391c0cb3.toInt(), 0x4ed8aa4a.toInt(), 0x5b9cca4f.toInt(), 0x682e6ff3.toInt(),
        0x748f82ee.toInt(), 0x78a5636f.toInt(), 0x84c87814.toInt(), 0x8cc70208.toInt(),
        0x90befffa.toInt(), 0xa4506ceb.toInt(), 0xbef9a3f7.toInt(), 0xc67178f2.toInt()
    )

}

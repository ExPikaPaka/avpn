package com.expikapaka.avpn

import com.expikapaka.avpn.encryption.SHA256
import org.junit.Assert.assertEquals
import org.junit.Test

class SHA256UnitTests {
    @Test
    fun sha256_64CharacterOutput_CorrectHash() {
        val sha256 = SHA256()
        val input = "Some kind of text"
        val expectedHash = "42926ddd6b349d26394a2c922eb054254e1fd9d071ffe155bc13bdecfeb05f7e"
        assertEquals(expectedHash, sha256.getSHA256(input))
    }

    @Test
    fun sha256_32CharacterOutput_CorrectHash() {
        val sha256 = SHA256()
        val input = "Some kind of text"
        val expectedHash = "42926ddd6b349d26394a2c922eb05425"
        assertEquals(expectedHash, sha256.getSHA128(input))
    }

    @Test
    fun sha256_16CharacterOutput_CorrectHash() {
        val sha256 = SHA256()
        val input = "Some kind of text"
        val expectedHash = "42926ddd6b349d26"
        assertEquals(expectedHash, sha256.getSHA64(input))
    }
}
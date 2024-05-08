package com.expikapaka.avpn

import com.expikapaka.avpn.encryption.AES128
import org.junit.Assert.assertArrayEquals
import org.junit.Test

class AES128UnitTests {

    @Test
    fun encryptAndDecrypt_CorrectOutput() {
        val aes = AES128()
        val key = "abcdefghijklmnop".toByteArray()
        val plainText = "Hello, AES!".toByteArray()

        val encrypted = aes.encrypt(plainText, key)
        val decrypted = aes.decrypt(encrypted, key)

        assertArrayEquals(plainText, decrypted)
    }

    @Test(expected = IllegalArgumentException::class)
    fun encrypt_WrongKeyLength_ExceptionThrown() {
        val aes = AES128()
        val key = "abc".toByteArray() // Invalid key length
        val plainText = "Hello, AES!".toByteArray()

        aes.encrypt(plainText, key)
    }

    @Test(expected = IllegalArgumentException::class)
    fun decrypt_WrongKeyLength_ExceptionThrown() {
        val aes = AES128()
        val key = "abc".toByteArray() // Invalid key length
        val encryptedText = byteArrayOf(1, 2, 3, 4) // Dummy encrypted text

        aes.decrypt(encryptedText, key)
    }

    @Test
    fun encryptAndDecrypt_EmptyInput_CorrectOutput() {
        val aes = AES128()
        val key = "abcdefghijklmnop".toByteArray()
        val plainText = ByteArray(0)

        val encrypted = aes.encrypt(plainText, key)
        val decrypted = aes.decrypt(encrypted, key)

        assertArrayEquals(plainText, decrypted)
    }

    @Test
    fun encryptAndDecrypt_LongInput_CorrectOutput() {
        val aes = AES128()
        val key = "abcdefghijklmnop".toByteArray()
        val plainText = "A very very very long input of some kind of text".toByteArray()

        val encrypted = aes.encrypt(plainText, key)
        val decrypted = aes.decrypt(encrypted, key)

        assertArrayEquals(plainText, decrypted)
    }
}
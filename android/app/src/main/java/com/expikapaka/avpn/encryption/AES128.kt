package com.expikapaka.avpn.encryption

import javax.crypto.Cipher
import javax.crypto.spec.SecretKeySpec


///////////// ABOUT THIS PACKAGE /////////////////////////////////////
//  This package contains AES128 class which provides methods for
//    encrypting and decrypting text using AES with a 128-bit key.
//
//  In the context of cryptography, AES (Advanced Encryption
//    Standard) is a widely used symmetric encryption algorithm.
//    This class utilizes AES in ECB (Electronic Codebook) mode with
//    PKCS5 padding.
//
//  Limitation: This implementation assumes a fixed key length of 16
//    bytes (128 bits). Ensure that the key provided is exactly 16
//    bytes long.
//
//////////////////////////////////////////////////////////////////////


// AES128 class with encrypt and decrypt functions
class AES128 {
    // Encrypts the plainBytes using AES with a 128-bit key
    fun encrypt(plainBytes: ByteArray, key: ByteArray): ByteArray {
        // Validate key length
        require(key.size == 16) { "Error: Key length must be 16 bytes (128 bits)." }

        // Create AES cipher instance with ECB mode and PKCS5 padding
        val cipher = Cipher.getInstance("AES/ECB/PKCS5Padding")
        val secretKeySpec = SecretKeySpec(key, "AES")
        cipher.init(Cipher.ENCRYPT_MODE, secretKeySpec)

        // Encrypt the plainBytes
        return cipher.doFinal(plainBytes)
    }

    // Decrypts the encryptedBytes using AES with a 128-bit key
    fun decrypt(encryptedBytes: ByteArray, key: ByteArray): ByteArray {
        // Validate key length
        require(key.size == 16) { "Error: Key length must be 16 bytes (128 bits)." }

        // Create AES cipher instance with ECB mode and PKCS5 padding
        val cipher = Cipher.getInstance("AES/ECB/PKCS5Padding")
        val secretKeySpec = SecretKeySpec(key, "AES")
        cipher.init(Cipher.DECRYPT_MODE, secretKeySpec)

        // Decrypt the encryptedBytes
        return cipher.doFinal(encryptedBytes)
    }
}
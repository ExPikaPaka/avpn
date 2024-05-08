package com.expikapaka.avpn

import com.expikapaka.avpn.encryption.DiffieHellman
import org.junit.Test

import org.junit.Assert.*



class DiffieHellmanUnitTests {
    @Test
    fun calculateSharedSecret_isCorrect() {
        val diffieHellman = DiffieHellman()
        val prime = diffieHellman.generatePrime(32)
        val privateKeyA = diffieHellman.generatePrivateKey(prime)
        val privateKeyB = diffieHellman.generatePrivateKey(prime)
        val primitiveRoot = diffieHellman.generatePrimitiveRoot(prime)
        val publicKeyA = diffieHellman.generatePublicKey(prime, primitiveRoot, privateKeyA)
        val publicKeyB = diffieHellman.generatePublicKey(prime, primitiveRoot, privateKeyB)
        val sharedSecretA = diffieHellman.calculateSharedSecret(publicKeyB, privateKeyA, prime)
        val sharedSecretB = diffieHellman.calculateSharedSecret(publicKeyA, privateKeyB, prime)
        assertEquals(sharedSecretA, sharedSecretB)
    }

}
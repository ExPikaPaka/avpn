package com.expikapaka.avpn.encryption

import kotlin.random.Random

class DiffieHellman {
    // Random number generator for generating primes and private keys
    private val random = Random(System.currentTimeMillis())

    // Generate a prime number of given bit length
    fun generatePrime(bitLength: Int): ULong {
        while (true) {
            // Generate a random number
            val value = generateRandomNumber(2UL, ULong.MAX_VALUE, bitLength)
            // Check if the generated number is prime
            if (isPrime(value)) {
                return value
            }
        }
    }

    // Generate a primitive root modulo a prime
    fun generatePrimitiveRoot(prime: ULong): ULong {
        while (true) {
            // Generate a random number
            val randomNumber = generateRandomNumber(2UL, prime - 1UL)
            // Check if the generated number is a primitive root modulo the prime
            if (isPrimitive(randomNumber, prime)) {
                return randomNumber
            }
        }
    }

    // Generate a private key in the range [2, p-1]
    fun generatePrivateKey(p: ULong): ULong {
        return generateRandomNumber(2UL, p - 1UL)
    }

    // Generate a public key using the formula (g^privateKey) % p
    fun generatePublicKey(p: ULong, g: ULong, privateKey: ULong): ULong {
        val result = modPow(g, privateKey, p)
        // Ensure the result is non-negative
        return if (result < 0UL) {
            result + p // Add p to make the result non-negative
        } else {
            result
        }
    }

    // Calculate the shared secret using the formula (serverPublicKey^privateKey) % p
    fun calculateSharedSecret(serverPublicKey: ULong, privateKey: ULong, p: ULong): ULong {
        return modPow(serverPublicKey, privateKey, p)
    }

    // Modular exponentiation function
    private fun modPow(base: ULong, exponent: ULong, modulus: ULong): ULong {
        var result = 1UL
        var b = base
        var e = exponent
        b %= modulus
        while (e > 0UL) {
            if (e and 1UL == 1UL) {
                result = (result * b) % modulus
            }
            b = (b * b) % modulus
            e = e shr 1
        }
        return result
    }

    // Generate a random number within a specified range
    private fun generateRandomNumber(min: ULong, max: ULong, bitLength: Int? = null): ULong {
        val range = max - min
        val randomBits = if (bitLength != null) {
            val maxBitLength = range.toString(2).length
            require(bitLength <= maxBitLength) { "Bit length exceeds range" }
            Random.nextBits(bitLength)
        } else {
            Random.nextBits(max.toString(2).length)
        }
        return min + randomBits.toUInt()
    }

    // Check if a number is prime
    private fun isPrime(n: ULong): Boolean {
        if (n <= 3UL) {
            // If n is 2 or 3, it's prime
            return true
        }

        // Check divisibility by 2 or 3
        if (n % 2UL == 0UL || n % 3UL == 0UL) {
            return false
        }

        var i = 5UL
        while (i * i <= n) {
            if (n % i == 0UL || n % (i + 2UL) == 0UL) {
                return false
            }
            i += 6UL
        }

        return true
    }

    // Check if a number is a primitive root modulo a prime
    private fun isPrimitive(g: ULong, p: ULong): Boolean {
        if (g < 2UL || g >= p) {
            return false
        }

        val factors = factorize(p - 1UL)

        for (factor in factors) {
            val power = (p - 1UL) / factor
            if (modPow(g, power, p) == 1UL) {
                return false
            }
        }

        return true
    }

    private fun factorize(n: ULong): List<ULong> {
        val factors = mutableListOf<ULong>()
        var number = n
        var factor: ULong = 2UL
        while (number > 1UL) {
            if (number % factor == 0UL) {
                factors.add(factor)
                number /= factor
            } else {
                factor++
            }
        }
        return factors
    }
}

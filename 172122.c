size_t olm_pk_encrypt(
    OlmPkEncryption *encryption,
    void const * plaintext, size_t plaintext_length,
    void * ciphertext, size_t ciphertext_length,
    void * mac, size_t mac_length,
    void * ephemeral_key, size_t ephemeral_key_size,
    const void * random, size_t random_length
) {
    if (ciphertext_length
            < olm_pk_ciphertext_length(encryption, plaintext_length)
        || mac_length
            < _olm_cipher_aes_sha_256_ops.mac_length(olm_pk_cipher)
        || ephemeral_key_size
            < olm_pk_key_length()) {
        encryption->last_error =
            OlmErrorCode::OLM_OUTPUT_BUFFER_TOO_SMALL;
        return std::size_t(-1);
    }
    if (random_length < olm_pk_encrypt_random_length(encryption)) {
        encryption->last_error =
            OlmErrorCode::OLM_NOT_ENOUGH_RANDOM;
        return std::size_t(-1);
    }

    _olm_curve25519_key_pair ephemeral_keypair;
    _olm_crypto_curve25519_generate_key((const uint8_t *) random, &ephemeral_keypair);
    olm::encode_base64(
        (const uint8_t *)ephemeral_keypair.public_key.public_key,
        CURVE25519_KEY_LENGTH,
        (uint8_t *)ephemeral_key
    );

    olm::SharedKey secret;
    _olm_crypto_curve25519_shared_secret(&ephemeral_keypair, &encryption->recipient_key, secret);
    size_t raw_ciphertext_length =
        _olm_cipher_aes_sha_256_ops.encrypt_ciphertext_length(olm_pk_cipher, plaintext_length);
    uint8_t *ciphertext_pos = (uint8_t *) ciphertext + ciphertext_length - raw_ciphertext_length;
    uint8_t raw_mac[MAC_LENGTH];
    size_t result = _olm_cipher_aes_sha_256_ops.encrypt(
        olm_pk_cipher,
        secret, sizeof(secret),
        (const uint8_t *) plaintext, plaintext_length,
        (uint8_t *) ciphertext_pos, raw_ciphertext_length,
        (uint8_t *) raw_mac, MAC_LENGTH
    );
    if (result != std::size_t(-1)) {
        olm::encode_base64(raw_mac, MAC_LENGTH, (uint8_t *)mac);
        olm::encode_base64(ciphertext_pos, raw_ciphertext_length, (uint8_t *)ciphertext);
    }
    return result;
}
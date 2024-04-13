size_t olm_pk_key_from_private(
    OlmPkDecryption * decryption,
    void * pubkey, size_t pubkey_length,
    const void * privkey, size_t privkey_length
) {
    if (pubkey_length < olm_pk_key_length()) {
        decryption->last_error =
            OlmErrorCode::OLM_OUTPUT_BUFFER_TOO_SMALL;
        return std::size_t(-1);
    }
    if (privkey_length < olm_pk_private_key_length()) {
        decryption->last_error =
            OlmErrorCode::OLM_INPUT_BUFFER_TOO_SMALL;
        return std::size_t(-1);
    }

    _olm_crypto_curve25519_generate_key((const uint8_t *) privkey, &decryption->key_pair);
    olm::encode_base64(
        (const uint8_t *)decryption->key_pair.public_key.public_key,
        CURVE25519_KEY_LENGTH,
        (uint8_t *)pubkey
    );
    return 0;
}
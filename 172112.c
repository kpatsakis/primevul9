size_t olm_pk_signing_key_from_seed(
    OlmPkSigning * signing,
    void * pubkey, size_t pubkey_length,
    const void * seed, size_t seed_length
) {
    if (pubkey_length < olm_pk_signing_public_key_length()) {
        signing->last_error =
            OlmErrorCode::OLM_OUTPUT_BUFFER_TOO_SMALL;
        return std::size_t(-1);
    }
    if (seed_length < olm_pk_signing_seed_length()) {
        signing->last_error =
            OlmErrorCode::OLM_INPUT_BUFFER_TOO_SMALL;
        return std::size_t(-1);
    }

    _olm_crypto_ed25519_generate_key((const uint8_t *) seed, &signing->key_pair);
    olm::encode_base64(
        (const uint8_t *)signing->key_pair.public_key.public_key,
        ED25519_PUBLIC_KEY_LENGTH,
        (uint8_t *)pubkey
    );
    return 0;
}
size_t olm_unpickle_pk_decryption(
    OlmPkDecryption * decryption,
    void const * key, size_t key_length,
    void *pickled, size_t pickled_length,
    void *pubkey, size_t pubkey_length
) {
    OlmPkDecryption & object = *decryption;
    if (pubkey != NULL && pubkey_length < olm_pk_key_length()) {
        object.last_error = OlmErrorCode::OLM_OUTPUT_BUFFER_TOO_SMALL;
        return std::size_t(-1);
    }
    std::uint8_t * const pos = reinterpret_cast<std::uint8_t *>(pickled);
    std::size_t raw_length = _olm_enc_input(
        reinterpret_cast<std::uint8_t const *>(key), key_length,
        pos, pickled_length, &object.last_error
    );
    if (raw_length == std::size_t(-1)) {
        return std::size_t(-1);
    }
    std::uint8_t * const end = pos + raw_length;
    /* On success unpickle will return (pos + raw_length). If unpickling
     * terminates too soon then it will return a pointer before
     * (pos + raw_length). On error unpickle will return (pos + raw_length + 1).
     */
    if (end != unpickle(pos, end + 1, object)) {
        if (object.last_error == OlmErrorCode::OLM_SUCCESS) {
            object.last_error = OlmErrorCode::OLM_CORRUPTED_PICKLE;
        }
        return std::size_t(-1);
    }
    if (pubkey != NULL) {
        olm::encode_base64(
            (const uint8_t *)object.key_pair.public_key.public_key,
            CURVE25519_KEY_LENGTH,
            (uint8_t *)pubkey
        );
    }
    return pickled_length;
}
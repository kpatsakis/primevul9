size_t olm_pk_sign(
    OlmPkSigning *signing,
    uint8_t const * message, size_t message_length,
    uint8_t * signature, size_t signature_length
) {
    if (signature_length < olm_pk_signature_length()) {
        signing->last_error = OlmErrorCode::OLM_OUTPUT_BUFFER_TOO_SMALL;
        return std::size_t(-1);
    }
    uint8_t *raw_sig = signature + olm_pk_signature_length() - ED25519_SIGNATURE_LENGTH;
    _olm_crypto_ed25519_sign(
        &signing->key_pair,
        message, message_length, raw_sig
    );
    olm::encode_base64(raw_sig, ED25519_SIGNATURE_LENGTH, signature);
    return olm_pk_signature_length();
}
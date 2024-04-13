size_t olm_pk_key_length(void) {
    return olm::encode_base64_length(CURVE25519_KEY_LENGTH);
}
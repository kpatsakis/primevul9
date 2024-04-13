size_t olm_pk_signing_public_key_length(void) {
    return olm::encode_base64_length(ED25519_PUBLIC_KEY_LENGTH);
}
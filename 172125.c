size_t olm_pk_signature_length(void) {
    return olm::encode_base64_length(ED25519_SIGNATURE_LENGTH);
}
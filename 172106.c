size_t olm_pk_ciphertext_length(
    OlmPkEncryption *encryption,
    size_t plaintext_length
) {
    return olm::encode_base64_length(
        _olm_cipher_aes_sha_256_ops.encrypt_ciphertext_length(olm_pk_cipher, plaintext_length)
    );
}
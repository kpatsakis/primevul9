size_t olm_pk_max_plaintext_length(
    OlmPkDecryption * decryption,
    size_t ciphertext_length
) {
    return _olm_cipher_aes_sha_256_ops.decrypt_max_plaintext_length(
        olm_pk_cipher, olm::decode_base64_length(ciphertext_length)
    );
}
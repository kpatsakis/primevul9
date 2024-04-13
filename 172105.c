size_t olm_pk_get_private_key(
    OlmPkDecryption * decryption,
    void *private_key, size_t private_key_length
) {
    if (private_key_length < olm_pk_private_key_length()) {
        decryption->last_error =
            OlmErrorCode::OLM_OUTPUT_BUFFER_TOO_SMALL;
        return std::size_t(-1);
    }
    std::memcpy(
        private_key,
        decryption->key_pair.private_key.private_key,
        olm_pk_private_key_length()
    );
    return olm_pk_private_key_length();
}
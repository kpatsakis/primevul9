size_t olm_pk_generate_key(
    OlmPkDecryption * decryption,
    void * pubkey, size_t pubkey_length,
    const void * privkey, size_t privkey_length
) {
    return olm_pk_key_from_private(decryption, pubkey, pubkey_length, privkey, privkey_length);
}
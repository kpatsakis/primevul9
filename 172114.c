OlmPkDecryption *olm_pk_decryption(
    void * memory
) {
    olm::unset(memory, sizeof(OlmPkDecryption));
    return new(memory) OlmPkDecryption;
}
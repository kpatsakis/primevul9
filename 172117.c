OlmPkEncryption *olm_pk_encryption(
    void * memory
) {
    olm::unset(memory, sizeof(OlmPkEncryption));
    return new(memory) OlmPkEncryption;
}
size_t olm_clear_pk_encryption(
    OlmPkEncryption *encryption
) {
    /* Clear the memory backing the encryption */
    olm::unset(encryption, sizeof(OlmPkEncryption));
    /* Initialise a fresh encryption object in case someone tries to use it */
    new(encryption) OlmPkEncryption();
    return sizeof(OlmPkEncryption);
}
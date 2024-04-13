size_t olm_clear_pk_decryption(
    OlmPkDecryption *decryption
) {
    /* Clear the memory backing the decryption */
    olm::unset(decryption, sizeof(OlmPkDecryption));
    /* Initialise a fresh decryption object in case someone tries to use it */
    new(decryption) OlmPkDecryption();
    return sizeof(OlmPkDecryption);
}
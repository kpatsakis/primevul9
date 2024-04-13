size_t olm_clear_pk_signing(OlmPkSigning *sign) {
    /* Clear the memory backing the signing */
    olm::unset(sign, sizeof(OlmPkSigning));
    /* Initialise a fresh signing object in case someone tries to use it */
    new(sign) OlmPkSigning();
    return sizeof(OlmPkSigning);
}
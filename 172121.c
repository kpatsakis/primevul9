OlmPkSigning *olm_pk_signing(void * memory) {
    olm::unset(memory, sizeof(OlmPkSigning));
    return new(memory) OlmPkSigning;
}
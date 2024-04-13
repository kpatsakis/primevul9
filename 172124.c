const char * olm_pk_encryption_last_error(
    OlmPkEncryption * encryption
) {
    auto error = encryption->last_error;
    return _olm_error_to_string(error);
}
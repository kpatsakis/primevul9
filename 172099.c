const char * olm_pk_decryption_last_error(
    OlmPkDecryption * decryption
) {
    auto error = decryption->last_error;
    return _olm_error_to_string(error);
}
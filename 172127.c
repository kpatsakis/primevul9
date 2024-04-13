const char * olm_pk_signing_last_error(OlmPkSigning * sign) {
    auto error = sign->last_error;
    return _olm_error_to_string(error);
}
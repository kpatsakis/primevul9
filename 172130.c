size_t olm_pickle_pk_decryption_length(
    OlmPkDecryption * decryption
) {
    return _olm_enc_output_length(pickle_length(*decryption));
}
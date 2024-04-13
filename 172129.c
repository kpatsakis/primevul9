size_t olm_pickle_pk_decryption(
    OlmPkDecryption * decryption,
    void const * key, size_t key_length,
    void *pickled, size_t pickled_length
) {
    OlmPkDecryption & object = *decryption;
    std::size_t raw_length = pickle_length(object);
    if (pickled_length < _olm_enc_output_length(raw_length)) {
        object.last_error = OlmErrorCode::OLM_OUTPUT_BUFFER_TOO_SMALL;
        return std::size_t(-1);
    }
    pickle(_olm_enc_output_pos(reinterpret_cast<std::uint8_t *>(pickled), raw_length), object);
    return _olm_enc_output(
        reinterpret_cast<std::uint8_t const *>(key), key_length,
        reinterpret_cast<std::uint8_t *>(pickled), raw_length
    );
}
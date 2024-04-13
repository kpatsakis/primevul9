    static std::size_t pickle_length(
        OlmPkDecryption const & value
    ) {
        std::size_t length = 0;
        length += olm::pickle_length(PK_DECRYPTION_PICKLE_VERSION);
        length += olm::pickle_length(value.key_pair);
        return length;
    }
    static std::uint8_t * pickle(
        std::uint8_t * pos,
        OlmPkDecryption const & value
    ) {
        pos = olm::pickle(pos, PK_DECRYPTION_PICKLE_VERSION);
        pos = olm::pickle(pos, value.key_pair);
        return pos;
    }
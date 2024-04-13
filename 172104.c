    static std::uint8_t const * unpickle(
        std::uint8_t const * pos, std::uint8_t const * end,
        OlmPkDecryption & value
    ) {
        uint32_t pickle_version;
        pos = olm::unpickle(pos, end, pickle_version);

        switch (pickle_version) {
        case 1:
            break;

        default:
            value.last_error = OlmErrorCode::OLM_UNKNOWN_PICKLE_VERSION;
            return end;
        }

        pos = olm::unpickle(pos, end, value.key_pair);
        return pos;
    }
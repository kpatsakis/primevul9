static int unserialize_data(struct usbredirparser_priv *parser,
                            uint8_t **pos, uint32_t *remain,
                            uint8_t **data, uint32_t *len_in_out,
                            const char *desc)
{
    uint32_t len;

    if (*remain < sizeof(uint32_t)) {
        ERROR("error buffer underrun while unserializing state");
        return -1;
    }
    memcpy(&len, *pos, sizeof(uint32_t));
    *pos += sizeof(uint32_t);
    *remain -= sizeof(uint32_t);

    if (*remain < len) {
        ERROR("error buffer underrun while unserializing state");
        return -1;
    }
    if (*data == NULL && len > 0) {
        *data = malloc(len);
        if (!*data) {
            ERROR("Out of memory allocating unserialize buffer");
            return -1;
        }
    } else {
        if (*len_in_out < len) {
            ERROR("error buffer overrun while unserializing state");
            return -1;
        }
    }

    memcpy(*data, *pos, len);
    *pos += len;
    *remain -= len;
    *len_in_out = len;

    DEBUG("unserialized %d bytes of %s data", len, desc);
    if (len >= 8)
        DEBUG("First 8 bytes of %s: %02x %02x %02x %02x %02x %02x %02x %02x",
              desc, (*data)[0], (*data)[1], (*data)[2], (*data)[3],
              (*data)[4], (*data)[5], (*data)[6], (*data)[7]);

    return 0;
}
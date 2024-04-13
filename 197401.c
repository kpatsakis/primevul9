static int serialize_data(struct usbredirparser_priv *parser,
                          uint8_t **state, uint8_t **pos, uint32_t *remain,
                          uint8_t *data, uint32_t len, const char *desc)
{
    DEBUG("serializing %d bytes of %s data", len, desc);
    if (len >= 8)
        DEBUG("First 8 bytes of %s: %02x %02x %02x %02x %02x %02x %02x %02x",
              desc, data[0], data[1], data[2], data[3],
                    data[4], data[5], data[6], data[7]);

    if (serialize_alloc(parser, state, pos, remain, sizeof(uint32_t) + len))
        return -1;

    memcpy(*pos, &len, sizeof(uint32_t));
    *pos += sizeof(uint32_t);
    *remain -= sizeof(uint32_t);

    memcpy(*pos, data, len);
    *pos += len;
    *remain -= len;

    return 0;
}
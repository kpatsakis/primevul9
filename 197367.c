static int serialize_int(struct usbredirparser_priv *parser,
                         uint8_t **state, uint8_t **pos, uint32_t *remain,
                         uint32_t val, const char *desc)
{
    DEBUG("serializing int %08x : %s", val, desc);

    if (serialize_alloc(parser, state, pos, remain, sizeof(uint32_t)))
        return -1;

    memcpy(*pos, &val, sizeof(uint32_t));
    *pos += sizeof(uint32_t);
    *remain -= sizeof(uint32_t);

    return 0;
}
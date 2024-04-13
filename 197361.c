static int unserialize_int(struct usbredirparser_priv *parser,
                           uint8_t **pos, uint32_t *remain, uint32_t *val,
                           const char *desc)
{
    if (*remain < sizeof(uint32_t)) {
        ERROR("error buffer underrun while unserializing state");
        return -1;
    }
    memcpy(val, *pos, sizeof(uint32_t));
    *pos += sizeof(uint32_t);
    *remain -= sizeof(uint32_t);

    DEBUG("unserialized int %08x : %s", *val, desc);

    return 0;
}
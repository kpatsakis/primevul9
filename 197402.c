static int serialize_alloc(struct usbredirparser_priv *parser,
                           uint8_t **state, uint8_t **pos,
                           uint32_t *remain, uint32_t needed)
{
    uint8_t *old_state = *state;
    uint32_t used, size;

    if (*remain >= needed)
        return 0;

    used = *pos - *state;
    size = (used + needed + USBREDIRPARSER_SERIALIZE_BUF_SIZE - 1) &
           ~(USBREDIRPARSER_SERIALIZE_BUF_SIZE - 1);

    *state = realloc(*state, size);
    if (!*state) {
        free(old_state);
        ERROR("Out of memory allocating serialization buffer");
        return -1;
    }

    *pos = *state + used;
    *remain = size - used;

    return 0;
}
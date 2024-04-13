static size_t item_make_header(const uint8_t nkey, const unsigned int flags, const int nbytes,
                     char *suffix, uint8_t *nsuffix) {
    if (settings.inline_ascii_response) {
        /* suffix is defined at 40 chars elsewhere.. */
        *nsuffix = (uint8_t) snprintf(suffix, 40, " %u %d\r\n", flags, nbytes - 2);
    } else {
        *nsuffix = sizeof(flags);
    }
    return sizeof(item) + nkey + *nsuffix + nbytes;
}
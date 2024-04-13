static inline int make_ascii_get_suffix(char *suffix, item *it, bool return_cas) {
    char *p;
    if (!settings.inline_ascii_response) {
        *suffix = ' ';
        p = itoa_u32(*((uint32_t *) ITEM_suffix(it)), suffix+1);
        *p = ' ';
        p = itoa_u32(it->nbytes-2, p+1);
    } else {
        p = suffix;
    }
    if (return_cas) {
        *p = ' ';
        p = itoa_u64(ITEM_get_cas(it), p+1);
    }
    *p = '\r';
    *(p+1) = '\n';
    *(p+2) = '\0';
    return (p - suffix) + 2;
}
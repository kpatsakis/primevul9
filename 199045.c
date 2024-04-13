item *do_item_touch(const char *key, size_t nkey, uint32_t exptime,
                    const uint32_t hv, conn *c) {
    item *it = do_item_get(key, nkey, hv, c, DO_UPDATE);
    if (it != NULL) {
        it->exptime = exptime;
    }
    return it;
}
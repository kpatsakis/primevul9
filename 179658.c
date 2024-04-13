static bstr *copy_or_wrap_mem(const void *data, size_t len, enum htp_alloc_strategy_t alloc) {
    if (data == NULL) return NULL;

    if (alloc == HTP_ALLOC_REUSE) {
        return bstr_wrap_mem(data, len);
    } else {
        return bstr_dup_mem(data, len);
    }
}
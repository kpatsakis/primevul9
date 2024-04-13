htp_status_t htp_tx_res_set_header(htp_tx_t *tx, const char *name, size_t name_len,
        const char *value, size_t value_len, enum htp_alloc_strategy_t alloc) {
    if ((tx == NULL) || (name == NULL) || (value == NULL)) return HTP_ERROR;


    htp_header_t *h = calloc(1, sizeof (htp_header_t));
    if (h == NULL) return HTP_ERROR;

    h->name = copy_or_wrap_mem(name, name_len, alloc);
    if (h->name == NULL) {
        free(h);
        return HTP_ERROR;
    }

    h->value = copy_or_wrap_mem(value, value_len, alloc);
    if (h->value == NULL) {
        bstr_free(h->name);
        free(h);
        return HTP_ERROR;
    }

    if (htp_table_add(tx->response_headers, h->name, h) != HTP_OK) {
        bstr_free(h->name);
        bstr_free(h->value);
        free(h);
        return HTP_ERROR;
    }

    return HTP_OK;
}
htp_status_t htp_tx_req_set_headers_clear(htp_tx_t *tx) {
    if ((tx == NULL) || (tx->request_headers == NULL)) return HTP_ERROR;

    htp_header_t *h = NULL;
    for (size_t i = 0, n = htp_table_size(tx->request_headers); i < n; i++) {
        h = htp_table_get_index(tx->request_headers, i, NULL);
        bstr_free(h->name);
        bstr_free(h->value);
        free(h);
    }

    htp_table_destroy(tx->request_headers);

    tx->request_headers = htp_table_create(32);
    if (tx->request_headers == NULL) return HTP_ERROR;

    return HTP_OK;
}
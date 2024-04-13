htp_status_t htp_tx_res_set_headers_clear(htp_tx_t *tx) {
    if ((tx == NULL) || (tx->response_headers == NULL)) return HTP_ERROR;

    htp_header_t *h = NULL;
    for (size_t i = 0, n = htp_table_size(tx->response_headers); i < n; i++) {
        h = htp_table_get_index(tx->response_headers, i, NULL);
        bstr_free(h->name);
        bstr_free(h->value);
        free(h);
    }

    htp_table_destroy(tx->response_headers);

    tx->response_headers = htp_table_create(32);
    if (tx->response_headers == NULL) return HTP_ERROR;

    return HTP_OK;
}
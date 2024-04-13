htp_status_t htp_tx_req_set_uri(htp_tx_t *tx, const char *uri, size_t uri_len, enum htp_alloc_strategy_t alloc) {
    if ((tx == NULL) || (uri == NULL)) return HTP_ERROR;

    tx->request_uri = copy_or_wrap_mem(uri, uri_len, alloc);
    if (tx->request_uri == NULL) return HTP_ERROR;

    return HTP_OK;
}
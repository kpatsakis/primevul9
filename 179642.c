htp_status_t htp_tx_req_set_method(htp_tx_t *tx, const char *method, size_t method_len, enum htp_alloc_strategy_t alloc) {
    if ((tx == NULL) || (method == NULL)) return HTP_ERROR;

    tx->request_method = copy_or_wrap_mem(method, method_len, alloc);
    if (tx->request_method == NULL) return HTP_ERROR;

    return HTP_OK;
}
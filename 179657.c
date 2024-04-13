htp_status_t htp_tx_res_set_status_message(htp_tx_t *tx, const char *msg, size_t msg_len, enum htp_alloc_strategy_t alloc) {
    if ((tx == NULL) || (msg == NULL)) return HTP_ERROR;

    if (tx->response_message != NULL) {
        bstr_free(tx->response_message);
    }

    tx->response_message = copy_or_wrap_mem(msg, msg_len, alloc);
    if (tx->response_message == NULL) return HTP_ERROR;

    return HTP_OK;
}
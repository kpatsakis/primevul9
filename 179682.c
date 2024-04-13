htp_status_t htp_tx_req_set_protocol(htp_tx_t *tx, const char *protocol, size_t protocol_len, enum htp_alloc_strategy_t alloc) {
    if ((tx == NULL) || (protocol == NULL)) return HTP_ERROR;

    tx->request_protocol = copy_or_wrap_mem(protocol, protocol_len, alloc);
    if (tx->request_protocol == NULL) return HTP_ERROR;

    return HTP_OK;
}
htp_status_t htp_tx_req_set_line(htp_tx_t *tx, const char *line, size_t line_len, enum htp_alloc_strategy_t alloc) {
    if ((tx == NULL) || (line == NULL) || (line_len == 0)) return HTP_ERROR;

    tx->request_line = copy_or_wrap_mem(line, line_len, alloc);
    if (tx->request_line == NULL) return HTP_ERROR;

    if (tx->connp->cfg->parse_request_line(tx->connp) != HTP_OK) return HTP_ERROR;

    return HTP_OK;
}
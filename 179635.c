htp_status_t htp_tx_state_response_complete(htp_tx_t *tx) {
    if (tx == NULL) return HTP_ERROR;
    return htp_tx_state_response_complete_ex(tx, 1 /* hybrid mode */);
}
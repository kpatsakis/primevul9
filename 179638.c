htp_status_t htp_tx_destroy(htp_tx_t *tx) {
    if (tx == NULL) return HTP_ERROR;

    if (!htp_tx_is_complete(tx)) return HTP_ERROR;

    htp_tx_destroy_incomplete(tx);

    return HTP_OK;
}
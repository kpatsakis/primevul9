htp_status_t htp_tx_state_request_start(htp_tx_t *tx) {
    if (tx == NULL) return HTP_ERROR;

    // Run hook REQUEST_START.
    htp_status_t rc = htp_hook_run_all(tx->connp->cfg->hook_request_start, tx);
    if (rc != HTP_OK) return rc;

    // Change state into request line parsing.
    tx->connp->in_state = htp_connp_REQ_LINE;
    tx->connp->in_tx->request_progress = HTP_REQUEST_LINE;

    return HTP_OK;
}
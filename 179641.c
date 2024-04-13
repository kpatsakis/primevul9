htp_status_t htp_tx_state_response_start(htp_tx_t *tx) {
    if (tx == NULL) return HTP_ERROR;

    tx->connp->out_tx = tx;

    // Run hook RESPONSE_START.
    htp_status_t rc = htp_hook_run_all(tx->connp->cfg->hook_response_start, tx);
    if (rc != HTP_OK) return rc;

    // Change state into response line parsing, except if we're following
    // a HTTP/0.9 request (no status line or response headers).
    if (tx->is_protocol_0_9) {
        tx->response_transfer_coding = HTP_CODING_IDENTITY;
        tx->response_content_encoding_processing = HTP_COMPRESSION_NONE;
        tx->response_progress = HTP_RESPONSE_BODY;
        tx->connp->out_state = htp_connp_RES_BODY_IDENTITY_STREAM_CLOSE;
        tx->connp->out_body_data_left = -1;
    } else {
        tx->connp->out_state = htp_connp_RES_LINE;
        tx->response_progress = HTP_RESPONSE_LINE;
    }

    return HTP_OK;
}
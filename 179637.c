htp_status_t htp_tx_state_request_complete(htp_tx_t *tx) {
    if (tx == NULL) return HTP_ERROR;

    if (tx->request_progress != HTP_REQUEST_COMPLETE) {
        htp_status_t rc = htp_tx_state_request_complete_partial(tx);
        if (rc != HTP_OK) return rc;
    }

    // Make a copy of the connection parser pointer, so that
    // we don't have to reference it via tx, which may be
    // destroyed later.
    htp_connp_t *connp = tx->connp;

    // Determine what happens next, and remove this transaction from the parser.
    if (tx->is_protocol_0_9) {
        connp->in_state = htp_connp_REQ_IGNORE_DATA_AFTER_HTTP_0_9;
    } else {
        connp->in_state = htp_connp_REQ_IDLE;
    }

    // Check if the entire transaction is complete. This call may
    // destroy the transaction, if auto-destroy is enabled.
    htp_tx_finalize(tx);

    // At this point, tx may no longer be valid.

    connp->in_tx = NULL;

    return HTP_OK;
}
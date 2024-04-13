htp_status_t htp_tx_state_response_complete_ex(htp_tx_t *tx, int hybrid_mode) {
    if (tx == NULL) return HTP_ERROR;

    if (tx->response_progress != HTP_RESPONSE_COMPLETE) {
        tx->response_progress = HTP_RESPONSE_COMPLETE;

        // Run the last RESPONSE_BODY_DATA HOOK, but only if there was a response body present.
        if (tx->response_transfer_coding != HTP_CODING_NO_BODY) {
            htp_tx_res_process_body_data_ex(tx, NULL, 0);
        }

        // Run hook RESPONSE_COMPLETE.
        htp_status_t rc = htp_hook_run_all(tx->connp->cfg->hook_response_complete, tx);
        if (rc != HTP_OK) return rc;
    }

    if (!hybrid_mode) {
        // Check if the inbound parser is waiting on us. If it is, that means that
        // there might be request data that the inbound parser hasn't consumed yet.
        // If we don't stop parsing we might encounter a response without a request,
        // which is why we want to return straight away before processing any data.
        //
        // This situation will occur any time the parser needs to see the server
        // respond to a particular situation before it can decide how to proceed. For
        // example, when a CONNECT is sent, different paths are used when it is accepted
        // and when it is not accepted.
        //
        // It is not enough to check only in_status here. Because of pipelining, it's possible
        // that many inbound transactions have been processed, and that the parser is
        // waiting on a response that we have not seen yet.
        if ((tx->connp->in_status == HTP_STREAM_DATA_OTHER) && (tx->connp->in_tx == tx->connp->out_tx)) {
            return HTP_DATA_OTHER;
        }

        // Do we have a signal to yield to inbound processing at
        // the end of the next transaction?
        if (tx->connp->out_data_other_at_tx_end) {
            // We do. Let's yield then.
            tx->connp->out_data_other_at_tx_end = 0;
            return HTP_DATA_OTHER;
        }
    }

    // Make a copy of the connection parser pointer, so that
    // we don't have to reference it via tx, which may be destroyed later.
    htp_connp_t *connp = tx->connp;

    // Finalize the transaction. This may call may destroy the transaction, if auto-destroy is enabled.
    htp_status_t rc = htp_tx_finalize(tx);
    if (rc != HTP_OK) return rc;

    // Disconnect transaction from the parser.
    connp->out_tx = NULL;

    connp->out_state = htp_connp_RES_IDLE;

    return HTP_OK;
}
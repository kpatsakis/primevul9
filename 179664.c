htp_status_t htp_tx_state_request_headers(htp_tx_t *tx) {
    if (tx == NULL) return HTP_ERROR;

    // If we're in HTP_REQ_HEADERS that means that this is the
    // first time we're processing headers in a request. Otherwise,
    // we're dealing with trailing headers.
    if (tx->request_progress > HTP_REQUEST_HEADERS) {
        // Request trailers.

        // Run hook HTP_REQUEST_TRAILER.
        htp_status_t rc = htp_hook_run_all(tx->connp->cfg->hook_request_trailer, tx);
        if (rc != HTP_OK) return rc;

        // Finalize sending raw header data.
        rc = htp_connp_req_receiver_finalize_clear(tx->connp);
        if (rc != HTP_OK) return rc;

        // Completed parsing this request; finalize it now.
        tx->connp->in_state = htp_connp_REQ_FINALIZE;
    } else if (tx->request_progress >= HTP_REQUEST_LINE) {
        // Request headers.

        // Did this request arrive in multiple data chunks?
        if (tx->connp->in_chunk_count != tx->connp->in_chunk_request_index) {
            tx->flags |= HTP_MULTI_PACKET_HEAD;
        }

        htp_status_t rc = htp_tx_process_request_headers(tx);
        if (rc != HTP_OK) return rc;

        tx->connp->in_state = htp_connp_REQ_CONNECT_CHECK;
    } else {
        htp_log(tx->connp, HTP_LOG_MARK, HTP_LOG_WARNING, 0, "[Internal Error] Invalid tx progress: %d", tx->request_progress);

        return HTP_ERROR;
    }

    return HTP_OK;
}
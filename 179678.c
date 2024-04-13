int htp_tx_is_complete(htp_tx_t *tx) {
    if (tx == NULL) return -1;

    // A transaction is considered complete only when both the request and
    // response are complete. (Sometimes a complete response can be seen
    // even while the request is ongoing.)
    if ((tx->request_progress != HTP_REQUEST_COMPLETE) || (tx->response_progress != HTP_RESPONSE_COMPLETE)) {
        return 0;
    } else {
        return 1;
    }
}
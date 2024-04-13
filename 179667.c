htp_status_t htp_tx_req_process_body_data_ex(htp_tx_t *tx, const void *data, size_t len) {
    if (tx == NULL) return HTP_ERROR;

    // NULL data is allowed in this private function; it's
    // used to indicate the end of request body.

    // Keep track of the body length.
    tx->request_entity_len += len;

    // Send data to the callbacks.

    htp_tx_data_t d;
    d.tx = tx;
    d.data = (unsigned char *) data;
    d.len = len;

    htp_status_t rc = htp_req_run_hook_body_data(tx->connp, &d);
    if (rc != HTP_OK) {
        htp_log(tx->connp, HTP_LOG_MARK, HTP_LOG_ERROR, 0, "Request body data callback returned error (%d)", rc);
        return HTP_ERROR;
    }

    return HTP_OK;
}
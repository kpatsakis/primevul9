htp_status_t htp_tx_state_response_line(htp_tx_t *tx) {
    if (tx == NULL) return HTP_ERROR;

    #if 0
    // Commented-out until we determine which fields can be
    // unavailable in real-life.

    // Unless we're dealing with HTTP/0.9, check that
    // the minimum amount of data has been provided.
    if (tx->is_protocol_0_9 != 0) {
        if ((tx->response_protocol == NULL) || (tx->response_status_number == -1) || (tx->response_message == NULL)) {
            return HTP_ERROR;
        }
    }
    #endif

    // Is the response line valid?
    if ((tx->response_protocol_number == HTP_PROTOCOL_INVALID)
            || (tx->response_status_number == HTP_STATUS_INVALID)
            || (tx->response_status_number < HTP_VALID_STATUS_MIN)
            || (tx->response_status_number > HTP_VALID_STATUS_MAX)) {
        htp_log(tx->connp, HTP_LOG_MARK, HTP_LOG_WARNING, 0, "Invalid response line.");
        tx->flags |= HTP_STATUS_LINE_INVALID;
    }

    // Run hook HTP_RESPONSE_LINE
    htp_status_t rc = htp_hook_run_all(tx->connp->cfg->hook_response_line, tx);
    if (rc != HTP_OK) return rc;

    return HTP_OK;
}
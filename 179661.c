static htp_status_t htp_tx_res_process_body_data_decompressor_callback(htp_tx_data_t *d) {
    if (d == NULL) return HTP_ERROR;

    #if HTP_DEBUG
    fprint_raw_data(stderr, __FUNCTION__, d->data, d->len);
    #endif

    // Keep track of actual response body length.
    d->tx->response_entity_len += d->len;

    // Invoke all callbacks.
    htp_status_t rc = htp_res_run_hook_body_data(d->tx->connp, d);
    if (rc != HTP_OK) return HTP_ERROR;

    return HTP_OK;
}
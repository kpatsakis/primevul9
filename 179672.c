htp_status_t htp_tx_state_request_complete_partial(htp_tx_t *tx) {
    if (tx == NULL) return HTP_ERROR;

    // Finalize request body.
    if (htp_tx_req_has_body(tx)) {
        htp_status_t rc = htp_tx_req_process_body_data_ex(tx, NULL, 0);
        if (rc != HTP_OK) return rc;
    }

    tx->request_progress = HTP_REQUEST_COMPLETE;

    // Run hook REQUEST_COMPLETE.
    htp_status_t rc = htp_hook_run_all(tx->connp->cfg->hook_request_complete, tx);
    if (rc != HTP_OK) return rc;

    // Clean-up.
    if (tx->connp->put_file != NULL) {
        bstr_free(tx->connp->put_file->filename);
        free(tx->connp->put_file);
        tx->connp->put_file = NULL;
    }

    return HTP_OK;
}
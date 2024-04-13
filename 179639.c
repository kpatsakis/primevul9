htp_tx_t *htp_tx_create(htp_connp_t *connp) {
    if (connp == NULL) return NULL;

    htp_tx_t *tx = calloc(1, sizeof (htp_tx_t));
    if (tx == NULL) return NULL;

    tx->connp = connp;
    tx->conn = connp->conn;
    tx->index = htp_list_size(tx->conn->transactions);
    tx->cfg = connp->cfg;
    tx->is_config_shared = HTP_CONFIG_SHARED;

    // Request fields.

    tx->request_progress = HTP_REQUEST_NOT_STARTED;
    tx->request_protocol_number = HTP_PROTOCOL_UNKNOWN;
    tx->request_content_length = -1;

    tx->parsed_uri_raw = htp_uri_alloc();
    if (tx->parsed_uri_raw == NULL) {
        htp_tx_destroy_incomplete(tx);
        return NULL;
    }

    tx->request_headers = htp_table_create(32);
    if (tx->request_headers == NULL) {
        htp_tx_destroy_incomplete(tx);
        return NULL;
    }

    tx->request_params = htp_table_create(32);
    if (tx->request_params == NULL) {
        htp_tx_destroy_incomplete(tx);
        return NULL;
    }

    // Response fields.

    tx->response_progress = HTP_RESPONSE_NOT_STARTED;
    tx->response_status = NULL;
    tx->response_status_number = HTP_STATUS_UNKNOWN;
    tx->response_protocol_number = HTP_PROTOCOL_UNKNOWN;
    tx->response_content_length = -1;

    tx->response_headers = htp_table_create(32);
    if (tx->response_headers == NULL) {
        htp_tx_destroy_incomplete(tx);
        return NULL;
    }

    htp_list_add(tx->conn->transactions, tx);

    return tx;
}
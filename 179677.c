int htp_tx_req_has_body(const htp_tx_t *tx) {
    if (tx == NULL) return -1;

    if ((tx->request_transfer_coding == HTP_CODING_IDENTITY) || (tx->request_transfer_coding == HTP_CODING_CHUNKED)) {
        return 1;
    }

    return 0;
}
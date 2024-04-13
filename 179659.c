void htp_tx_req_set_parsed_uri(htp_tx_t *tx, htp_uri_t *parsed_uri) {
    if ((tx == NULL) || (parsed_uri == NULL)) return;

    if (tx->parsed_uri != NULL) {
        htp_uri_free(tx->parsed_uri);
    }

    tx->parsed_uri = parsed_uri;
}
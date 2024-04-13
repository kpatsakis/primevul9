void htp_tx_req_set_protocol_number(htp_tx_t *tx, int protocol_number) {
    if (tx == NULL) return;
    tx->request_protocol_number = protocol_number;
}
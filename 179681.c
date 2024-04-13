void htp_tx_res_set_protocol_number(htp_tx_t *tx, int protocol_number) {
    if (tx == NULL) return;
    tx->response_protocol_number = protocol_number;
}
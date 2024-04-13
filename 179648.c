void htp_tx_req_set_protocol_0_9(htp_tx_t *tx, int is_protocol_0_9) {
    if (tx == NULL) return;

    if (is_protocol_0_9) {
        tx->is_protocol_0_9 = 1;
    } else {
        tx->is_protocol_0_9 = 0;
    }
}
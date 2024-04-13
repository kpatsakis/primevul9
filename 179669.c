void htp_tx_req_set_method_number(htp_tx_t *tx, enum htp_method_t method_number) {
    if (tx == NULL) return;
    tx->request_method_number = method_number;
}
void htp_tx_res_set_status_code(htp_tx_t *tx, int status_code) {
    if (tx == NULL) return;
    tx->response_status_number = status_code;
}
void htp_tx_set_user_data(htp_tx_t *tx, void *user_data) {
    if (tx == NULL) return;
    tx->user_data = user_data;
}
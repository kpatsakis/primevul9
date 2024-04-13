void *htp_tx_get_user_data(const htp_tx_t *tx) {
    if (tx == NULL) return NULL;
    return tx->user_data;
}
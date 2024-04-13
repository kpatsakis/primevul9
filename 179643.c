int htp_tx_get_is_config_shared(const htp_tx_t *tx) {
    if (tx == NULL) return -1;
    return tx->is_config_shared;
}
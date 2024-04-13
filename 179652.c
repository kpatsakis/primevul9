void htp_tx_register_request_body_data(htp_tx_t *tx, int (*callback_fn)(htp_tx_data_t *)) {
    if ((tx == NULL) || (callback_fn == NULL)) return;
    htp_hook_register(&tx->hook_request_body_data, (htp_callback_fn_t) callback_fn);
}
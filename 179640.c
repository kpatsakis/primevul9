htp_param_t *htp_tx_req_get_param(htp_tx_t *tx, const char *name, size_t name_len) {
    if ((tx == NULL) || (name == NULL)) return NULL;
    return htp_table_get_mem(tx->request_params, name, name_len);
}
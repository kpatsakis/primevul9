htp_status_t htp_tx_res_process_body_data(htp_tx_t *tx, const void *data, size_t len) {
    if ((tx == NULL) || (data == NULL)) return HTP_ERROR;
    if (len == 0) return HTP_OK;
    return htp_tx_res_process_body_data_ex(tx, data, len);
}
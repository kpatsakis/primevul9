htp_status_t htp_tx_req_add_param(htp_tx_t *tx, htp_param_t *param) {
    if ((tx == NULL) || (param == NULL)) return HTP_ERROR;

    if (tx->cfg->parameter_processor != NULL) {
        if (tx->cfg->parameter_processor(param) != HTP_OK) return HTP_ERROR;
    }

    return htp_table_addk(tx->request_params, param->name, param);
}
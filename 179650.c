htp_param_t *htp_tx_req_get_param_ex(htp_tx_t *tx, enum htp_data_source_t source, const char *name, size_t name_len) {
    if ((tx == NULL) || (name == NULL)) return NULL;

    htp_param_t *p = NULL;

    for (size_t i = 0, n = htp_table_size(tx->request_params); i < n; i++) {
        p = htp_table_get_index(tx->request_params, i, NULL);
        if (p->source != source) continue;

        if (bstr_cmp_mem_nocase(p->name, name, name_len) == 0) return p;
    }

    return NULL;
}
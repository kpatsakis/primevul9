void htp_tx_destroy_incomplete(htp_tx_t *tx) {
    if (tx == NULL) return;

    // Disconnect transaction from other structures.
    htp_conn_remove_tx(tx->conn, tx);
    htp_connp_tx_remove(tx->connp, tx);

    // Request fields.

    bstr_free(tx->request_line);
    bstr_free(tx->request_method);
    bstr_free(tx->request_uri);
    bstr_free(tx->request_protocol);
    bstr_free(tx->request_content_type);
    bstr_free(tx->request_hostname);
    htp_uri_free(tx->parsed_uri_raw);
    htp_uri_free(tx->parsed_uri);
    bstr_free(tx->request_auth_username);
    bstr_free(tx->request_auth_password);

    // Request_headers.
    if (tx->request_headers != NULL) {
        htp_header_t *h = NULL;
        for (size_t i = 0, n = htp_table_size(tx->request_headers); i < n; i++) {
            h = htp_table_get_index(tx->request_headers, i, NULL);
            bstr_free(h->name);
            bstr_free(h->value);
            free(h);
        }

        htp_table_destroy(tx->request_headers);
    }

    // Request parsers.

    htp_urlenp_destroy(tx->request_urlenp_query);
    htp_urlenp_destroy(tx->request_urlenp_body);
    htp_mpartp_destroy(tx->request_mpartp);

    // Request parameters.

    htp_param_t *param = NULL;
    for (size_t i = 0, n = htp_table_size(tx->request_params); i < n; i++) {
        param = htp_table_get_index(tx->request_params, i, NULL);
        bstr_free(param->name);
        bstr_free(param->value);
        free(param);
    }

    htp_table_destroy(tx->request_params);

    // Request cookies.

    if (tx->request_cookies != NULL) {
        bstr *b = NULL;
        for (size_t i = 0, n = htp_table_size(tx->request_cookies); i < n; i++) {
            b = htp_table_get_index(tx->request_cookies, i, NULL);
            bstr_free(b);
        }

        htp_table_destroy(tx->request_cookies);
    }

    htp_hook_destroy(tx->hook_request_body_data);

    // Response fields.

    bstr_free(tx->response_line);
    bstr_free(tx->response_protocol);
    bstr_free(tx->response_status);
    bstr_free(tx->response_message);
    bstr_free(tx->response_content_type);

    // Destroy response headers.
    if (tx->response_headers != NULL) {
        htp_header_t *h = NULL;
        for (size_t i = 0, n = htp_table_size(tx->response_headers); i < n; i++) {
            h = htp_table_get_index(tx->response_headers, i, NULL);
            bstr_free(h->name);
            bstr_free(h->value);
            free(h);
        }

        htp_table_destroy(tx->response_headers);
    }

    // If we're using a private configuration structure, destroy it.
    if (tx->is_config_shared == HTP_CONFIG_PRIVATE) {
        htp_config_destroy(tx->cfg);
    }

    free(tx);
}
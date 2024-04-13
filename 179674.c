htp_status_t htp_tx_state_response_headers(htp_tx_t *tx) {
    if (tx == NULL) return HTP_ERROR;

    // Check for compression.

    // Determine content encoding.

    tx->response_content_encoding = HTP_COMPRESSION_NONE;

    htp_header_t *ce = htp_table_get_c(tx->response_headers, "content-encoding");
    if (ce != NULL) {
        if ((bstr_cmp_c_nocase(ce->value, "gzip") == 0) || (bstr_cmp_c_nocase(ce->value, "x-gzip") == 0)) {
            tx->response_content_encoding = HTP_COMPRESSION_GZIP;
        } else if ((bstr_cmp_c_nocase(ce->value, "deflate") == 0) || (bstr_cmp_c_nocase(ce->value, "x-deflate") == 0)) {
            tx->response_content_encoding = HTP_COMPRESSION_DEFLATE;
        } else if (bstr_cmp_c_nocase(ce->value, "inflate") != 0) {
            htp_log(tx->connp, HTP_LOG_MARK, HTP_LOG_WARNING, 0, "Unknown response content encoding");
        }
    }

    // Configure decompression, if enabled in the configuration.
    if (tx->connp->cfg->response_decompression_enabled) {
        tx->response_content_encoding_processing = tx->response_content_encoding;
    } else {
        tx->response_content_encoding_processing = HTP_COMPRESSION_NONE;
    }

    // Finalize sending raw header data.
    htp_status_t rc = htp_connp_res_receiver_finalize_clear(tx->connp);
    if (rc != HTP_OK) return rc;

    // Run hook RESPONSE_HEADERS.
    rc = htp_hook_run_all(tx->connp->cfg->hook_response_headers, tx);
    if (rc != HTP_OK) return rc;

    // Initialize the decompression engine as necessary. We can deal with three
    // scenarios:
    //
    // 1. Decompression is enabled, compression indicated in headers, and we decompress.
    //
    // 2. As above, but the user disables decompression by setting response_content_encoding
    //    to COMPRESSION_NONE.
    //
    // 3. Decompression is disabled and we do not attempt to enable it, but the user
    //    forces decompression by setting response_content_encoding to one of the
    //    supported algorithms.
    if ((tx->response_content_encoding_processing == HTP_COMPRESSION_GZIP) || (tx->response_content_encoding_processing == HTP_COMPRESSION_DEFLATE)) {
        if (tx->connp->out_decompressor != NULL) {
            tx->connp->out_decompressor->destroy(tx->connp->out_decompressor);
            tx->connp->out_decompressor = NULL;
        }

        tx->connp->out_decompressor = htp_gzip_decompressor_create(tx->connp, tx->response_content_encoding_processing);
        if (tx->connp->out_decompressor == NULL) return HTP_ERROR;
        
        tx->connp->out_decompressor->callback = htp_tx_res_process_body_data_decompressor_callback;
    } else if (tx->response_content_encoding_processing != HTP_COMPRESSION_NONE) {
        return HTP_ERROR;
    }

    return HTP_OK;
}
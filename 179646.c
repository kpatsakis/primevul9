static htp_status_t htp_tx_process_request_headers(htp_tx_t *tx) {
    if (tx == NULL) return HTP_ERROR;

    // Determine if we have a request body, and how it is packaged.

    htp_status_t rc = HTP_OK;

    htp_header_t *cl = htp_table_get_c(tx->request_headers, "content-length");
    htp_header_t *te = htp_table_get_c(tx->request_headers, "transfer-encoding");

    // Check for the Transfer-Encoding header, which would indicate a chunked request body.
    if (te != NULL) {
        // Make sure it contains "chunked" only.
        // TODO The HTTP/1.1 RFC also allows the T-E header to contain "identity", which
        //      presumably should have the same effect as T-E header absence. However, Apache
        //      (2.2.22 on Ubuntu 12.04 LTS) instead errors out with "Unknown Transfer-Encoding: identity".
        //      And it behaves strangely, too, sending a 501 and proceeding to process the request
        //      (e.g., PHP is run), but without the body. It then closes the connection.
        if (bstr_cmp_c_nocase(te->value, "chunked") != 0) {
            // Invalid T-E header value.
            tx->request_transfer_coding = HTP_CODING_INVALID;
            tx->flags |= HTP_REQUEST_INVALID_T_E;
            tx->flags |= HTP_REQUEST_INVALID;
        } else {
            // Chunked encoding is a HTTP/1.1 feature, so check that an earlier protocol
            // version is not used. The flag will also be set if the protocol could not be parsed.
            //
            // TODO IIS 7.0, for example, would ignore the T-E header when it
            //      it is used with a protocol below HTTP 1.1. This should be a
            //      personality trait.
            if (tx->request_protocol_number < HTP_PROTOCOL_1_1) {
                tx->flags |= HTP_REQUEST_INVALID_T_E;
                tx->flags |= HTP_REQUEST_SMUGGLING;
            }

            // If the T-E header is present we are going to use it.
            tx->request_transfer_coding = HTP_CODING_CHUNKED;

            // We are still going to check for the presence of C-L.
            if (cl != NULL) {
                // According to the HTTP/1.1 RFC (section 4.4):
                //
                // "The Content-Length header field MUST NOT be sent
                //  if these two lengths are different (i.e., if a Transfer-Encoding
                //  header field is present). If a message is received with both a
                //  Transfer-Encoding header field and a Content-Length header field,
                //  the latter MUST be ignored."
                //
                tx->flags |= HTP_REQUEST_SMUGGLING;
            }
        }
    } else if (cl != NULL) {
        // Check for a folded C-L header.
        if (cl->flags & HTP_FIELD_FOLDED) {
            tx->flags |= HTP_REQUEST_SMUGGLING;
        }

        // Check for multiple C-L headers.
        if (cl->flags & HTP_FIELD_REPEATED) {
            tx->flags |= HTP_REQUEST_SMUGGLING;
            // TODO Personality trait to determine which C-L header to parse.
            //      At the moment we're parsing the combination of all instances,
            //      which is bound to fail (because it will contain commas).
        }

        // Get the body length.
        tx->request_content_length = htp_parse_content_length(cl->value);
        if (tx->request_content_length < 0) {
            tx->request_transfer_coding = HTP_CODING_INVALID;
            tx->flags |= HTP_REQUEST_INVALID_C_L;
            tx->flags |= HTP_REQUEST_INVALID;
        } else {
            // We have a request body of known length.
            tx->request_transfer_coding = HTP_CODING_IDENTITY;
        }
    } else {
        // No body.
        tx->request_transfer_coding = HTP_CODING_NO_BODY;
    }

    // If we could not determine the correct body handling,
    // consider the request invalid.
    if (tx->request_transfer_coding == HTP_CODING_UNKNOWN) {
        tx->request_transfer_coding = HTP_CODING_INVALID;
        tx->flags |= HTP_REQUEST_INVALID;
    }

    // Check for PUT requests, which we need to treat as file uploads.
    if (tx->request_method_number == HTP_M_PUT) {
        if (htp_tx_req_has_body(tx)) {
            // Prepare to treat PUT request body as a file.
            
            tx->connp->put_file = calloc(1, sizeof (htp_file_t));
            if (tx->connp->put_file == NULL) return HTP_ERROR;

            tx->connp->put_file->fd = -1;
            tx->connp->put_file->source = HTP_FILE_PUT;
        } else {
            // TODO Warn about PUT request without a body.
        }
    }

    // Determine hostname.

    // Use the hostname from the URI, when available.   
    if (tx->parsed_uri->hostname != NULL) {
        tx->request_hostname = bstr_dup(tx->parsed_uri->hostname);
        if (tx->request_hostname == NULL) return HTP_ERROR;
    }

    tx->request_port_number = tx->parsed_uri->port_number;

    // Examine the Host header.

    htp_header_t *h = htp_table_get_c(tx->request_headers, "host");
    if (h == NULL) {
        // No host information in the headers.

        // HTTP/1.1 requires host information in the headers.
        if (tx->request_protocol_number >= HTP_PROTOCOL_1_1) {
            tx->flags |= HTP_HOST_MISSING;
        }
    } else {
        // Host information available in the headers.

        bstr *hostname;
        int port;

        rc = htp_parse_header_hostport(h->value, &hostname, NULL, &port, &(tx->flags));
        if (rc != HTP_OK) return rc;

        if (hostname != NULL) {
            // The host information in the headers is valid.

            // Is there host information in the URI?
            if (tx->request_hostname == NULL) {
                // There is no host information in the URI. Place the
                // hostname from the headers into the parsed_uri structure.
                tx->request_hostname = hostname;
                tx->request_port_number = port;
            } else {
                // The host information appears in the URI and in the headers. The
                // HTTP RFC states that we should ignore the header copy.
                
                // Check for different hostnames.
                if (bstr_cmp_nocase(hostname, tx->request_hostname) != 0) {                    
                    tx->flags |= HTP_HOST_AMBIGUOUS;
                }

                // Check for different ports.
                if (((tx->request_port_number != -1)&&(port != -1))&&(tx->request_port_number != port)) {
                    tx->flags |= HTP_HOST_AMBIGUOUS;
                }

                bstr_free(hostname);
            }
        } else {
            // Invalid host information in the headers.

            if (tx->request_hostname != NULL) {
                // Raise the flag, even though the host information in the headers is invalid.
                tx->flags |= HTP_HOST_AMBIGUOUS;
            }
        }
    }

    // Determine Content-Type.
    htp_header_t *ct = htp_table_get_c(tx->request_headers, "content-type");
    if (ct != NULL) {
        rc = htp_parse_ct_header(ct->value, &tx->request_content_type);
        if (rc != HTP_OK) return rc;
    }

    // Parse cookies.
    if (tx->connp->cfg->parse_request_cookies) {
        rc = htp_parse_cookies_v0(tx->connp);
        if (rc != HTP_OK) return rc;
    }

    // Parse authentication information.
    if (tx->connp->cfg->parse_request_auth) {
        rc = htp_parse_authorization(tx->connp);
        if (rc == HTP_DECLINED) {
            // Don't fail the stream if an authorization header is invalid, just set a flag.
            tx->flags |= HTP_AUTH_INVALID;
        } else {
            if (rc != HTP_OK) return rc;
        }
    }

    // Finalize sending raw header data.
    rc = htp_connp_req_receiver_finalize_clear(tx->connp);
    if (rc != HTP_OK) return rc;

    // Run hook REQUEST_HEADERS.
    rc = htp_hook_run_all(tx->connp->cfg->hook_request_headers, tx);
    if (rc != HTP_OK) return rc;

    // We cannot proceed if the request is invalid.
    if (tx->flags & HTP_REQUEST_INVALID) {
        return HTP_ERROR;
    }

    return HTP_OK;
}
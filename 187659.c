static int parse_connection(struct transaction_t *txn)
{
    const char **conn = spool_getheader(txn->req_hdrs, "Connection");
    int i;

    if (conn && txn->flags.ver == VER_2) {
        txn->error.desc = "Connection not allowed in HTTP/2";
        return HTTP_BAD_REQUEST;
    }

    if (!httpd_timeout || txn->flags.ver == VER_1_0) {
        /* Non-persistent connection by default */
        txn->flags.conn |= CONN_CLOSE;
    }

    /* Look for interesting connection tokens */
    for (i = 0; conn && conn[i]; i++) {
        tok_t tok = TOK_INITIALIZER(conn[i], ",", TOK_TRIMLEFT|TOK_TRIMRIGHT);
        char *token;

        while ((token = tok_next(&tok))) {
            switch (txn->flags.ver) {
            case VER_1_1:
                if (!strcasecmp(token, "Upgrade")) {
                    /* Client wants to upgrade */
                    const char **upgrade =
                        spool_getheader(txn->req_hdrs, "Upgrade");

                    if (upgrade && upgrade[0]) {
                        if (!txn->conn->tls_ctx && tls_enabled() &&
                            !strncasecmp(upgrade[0], TLS_VERSION,
                                         strcspn(upgrade[0], " ,"))) {
                            /* Upgrade to TLS */
                            txn->flags.conn |= CONN_UPGRADE;
                            txn->flags.upgrade |= UPGRADE_TLS;
                        }
                        else if (http2_enabled() &&
                                 !strncasecmp(upgrade[0],
                                              NGHTTP2_CLEARTEXT_PROTO_VERSION_ID,
                                              strcspn(upgrade[0], " ,"))) {
                            /* Upgrade to HTTP/2 */
                            txn->flags.conn |= CONN_UPGRADE;
                            txn->flags.upgrade |= UPGRADE_HTTP2;
                        }
                        else if (ws_enabled() &&
                                 !strncasecmp(upgrade[0], WS_TOKEN,
                                              strcspn(upgrade[0], " ,"))) {
                            /* Upgrade to WebSockets */
                            txn->flags.conn |= CONN_UPGRADE;
                            txn->flags.upgrade |= UPGRADE_WS;
                        }
                        else {
                            /* Unknown/unsupported protocol - no upgrade */
                        }
                    }
                }
                else if (!strcasecmp(token, "close")) {
                    /* Non-persistent connection */
                    txn->flags.conn |= CONN_CLOSE;
                }
                break;

            case VER_1_0:
                if (httpd_timeout && !strcasecmp(token, "keep-alive")) {
                    /* Persistent connection */
                    txn->flags.conn = CONN_KEEPALIVE;
                }
                break;
            }
        }

        tok_fini(&tok);
    }

    return 0;
}
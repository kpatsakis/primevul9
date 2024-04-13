static int preauth_check_hdrs(struct transaction_t *txn)
{
    int ret = 0;
    const char **hdr;

    if (txn->flags.redirect) return 0;

    /* Check for mandatory Host header (HTTP/1.1+ only) */
    if ((hdr = spool_getheader(txn->req_hdrs, "Host"))) {
        if (hdr[1]) {
            txn->error.desc = "Too many Host headers";
            return HTTP_BAD_REQUEST;
        }

        /* Create an :authority pseudo header from Host */
        spool_cache_header(xstrdup(":authority"),
                           xstrdup(hdr[0]), txn->req_hdrs);
    }
    else {
        switch (txn->flags.ver) {
        case VER_2:
            /* HTTP/2 - check for :authority pseudo header */
            if (spool_getheader(txn->req_hdrs, ":authority")) break;

            /* Fall through and create an :authority pseudo header */
            GCC_FALLTHROUGH

        case VER_1_0:
            /* HTTP/1.0 - create an :authority pseudo header from URI */
            if (txn->req_uri->server) {
                buf_setcstr(&txn->buf, txn->req_uri->server);
                if (txn->req_uri->port)
                    buf_printf(&txn->buf, ":%d", txn->req_uri->port);
            }
            else buf_setcstr(&txn->buf, config_servername);

            spool_cache_header(xstrdup(":authority"),
                               buf_release(&txn->buf), txn->req_hdrs);
            break;

        case VER_1_1:
        default:
            txn->error.desc = "Missing Host header";
            return HTTP_BAD_REQUEST;
        }
    }

    /* Check message framing */
    if ((ret = http_parse_framing(txn->flags.ver == VER_2, txn->req_hdrs,
                                  &txn->req_body, &txn->error.desc))) return ret;

    /* Check for Expectations */
    if ((ret = parse_expect(txn))) return ret;

    /* Check for Connection options */
    if ((ret = parse_connection(txn))) return ret;

    syslog(LOG_DEBUG, "conn flags: %#x  upgrade flags: %#x  tls req: %d",
           txn->flags.conn, txn->flags.upgrade, httpd_tls_required);
    if (txn->flags.conn & CONN_UPGRADE) {
        /* Read any request body (can't upgrade in middle of request) */
        txn->req_body.flags |= BODY_DECODE;
        ret = http_read_req_body(txn);
        if (ret) {
            txn->flags.conn = CONN_CLOSE;
            return ret;
        }

        if (txn->flags.upgrade & UPGRADE_TLS) {
            if ((ret = starttls(txn, NULL))) {
                txn->flags.conn = CONN_CLOSE;
                return ret;
            }

            /* Don't advertise TLS Upgrade anymore */
            txn->flags.upgrade &= ~UPGRADE_TLS;
        }

        syslog(LOG_DEBUG, "upgrade flags: %#x  tls req: %d",
               txn->flags.upgrade, httpd_tls_required);
        if ((txn->flags.upgrade & UPGRADE_HTTP2) && !httpd_tls_required) {
            if ((ret = http2_start_session(txn, NULL))) {
                txn->flags.conn = CONN_CLOSE;
                return ret;
            }

            /* Upgrade header field mechanism not available under HTTP/2 */
            txn->flags.upgrade = 0;
        }
    }
    else if (!txn->conn->tls_ctx && txn->flags.ver == VER_1_1) {
        /* Advertise available upgrade protocols */
        if (tls_enabled() &&
            config_mupdate_server && config_getstring(IMAPOPT_PROXYSERVERS)) {
            txn->flags.upgrade |= UPGRADE_TLS;
        }
        if (http2_enabled()) txn->flags.upgrade |= UPGRADE_HTTP2;
    }

    if (txn->flags.upgrade) txn->flags.conn |= CONN_UPGRADE;
    else txn->flags.conn &= ~CONN_UPGRADE;

    return 0;
}
static void cmdloop(struct http_connection *conn)
{
    struct transaction_t txn;

    /* Start with an empty (clean) transaction */
    memset(&txn, 0, sizeof(struct transaction_t));
    txn.conn = conn;

    if (config_getswitch(IMAPOPT_HTTPALLOWCOMPRESS)) {
        txn.zstrm = zlib_init();
        txn.zstd = zstd_init();
        txn.brotli = brotli_init();
    }

    /* Enable command timer */
    cmdtime_settimer(1);

    /* Enable provisional responses for long-running mailbox ops */
    mailbox_set_wait_cb((mailbox_wait_cb_t *) &keepalive_response, &txn);

    do {
        int ret = 0;

        /* Reset txn state */
        transaction_reset(&txn);

        /* make sure nothing leaked */
        assert(!open_mailboxes_exist());

        /* Check for input from client */
        do {
            /* Flush any buffered output */
            prot_flush(httpd_out);
            if (backend_current) prot_flush(backend_current->out);

            /* Check for shutdown file */
            if (shutdown_file(txn.buf.s, txn.buf.alloc) ||
                (httpd_userid &&
                 userdeny(httpd_userid, config_ident, txn.buf.s, txn.buf.alloc))) {
                txn.error.desc = txn.buf.s;
                txn.flags.conn = CONN_CLOSE;
                ret = HTTP_SHUTDOWN;
                break;
            }

            signals_poll();

        } while (!proxy_check_input(protin, httpd_in, httpd_out,
                                    backend_current ? backend_current->in : NULL,
                                    NULL, 30));

        
        /* Start command timer */
        cmdtime_starttimer();

        if (txn.conn->sess_ctx) {
            /* HTTP/2 input */
            http2_input(&txn);
        }
        else if (txn.ws_ctx) {
            /* WebSocket over HTTP/1.1 input */
            ws_input(&txn);
        }
        else if (!ret) {
            /* HTTP/1.x request */
            http1_input(&txn);
        }

        if (ret == HTTP_SHUTDOWN) {
            syslog(LOG_WARNING,
                   "Shutdown file: \"%s\", closing connection", txn.error.desc);
            protgroup_free(protin);
            shut_down(0);
        }

    } while (!(txn.flags.conn & CONN_CLOSE));

    /* Memory cleanup */
    transaction_free(&txn);
}
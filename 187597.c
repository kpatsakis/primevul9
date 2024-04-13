EXPORTED void error_response(long code, struct transaction_t *txn)
{
    struct buf *html = &txn->resp_body.payload;

    /* Neither Brief nor Prefer affect error response bodies */
    txn->flags.vary &= ~(VARY_BRIEF | VARY_PREFER);
    txn->resp_body.prefs = 0;

#ifdef WITH_DAV
    if (code != HTTP_UNAUTHORIZED && txn->error.precond) {
        xmlNodePtr root = xml_add_error(NULL, &txn->error, NULL);

        if (root) {
            xml_response(code, txn, root->doc);
            xmlFreeDoc(root->doc);
            return;
        }
    }
#endif /* WITH_DAV */

    if (!txn->error.desc) {
        switch (code) {
            /* 4xx codes */
        case HTTP_BAD_REQUEST:
            txn->error.desc =
                "The request was not understood by this server.";
            break;

        case HTTP_NOT_FOUND:
            txn->error.desc =
                "The requested URL was not found on this server.";
            break;

        case HTTP_NOT_ALLOWED:
            txn->error.desc =
                "The requested method is not allowed for the URL.";
            break;

        case HTTP_GONE:
            txn->error.desc =
                "The requested URL has been removed from this server.";
            break;

            /* 5xx codes */
        case HTTP_SERVER_ERROR:
            txn->error.desc =
                "The server encountered an internal error.";
            break;

        case HTTP_NOT_IMPLEMENTED:
            txn->error.desc =
                "The requested method is not implemented by this server.";
            break;

        case HTTP_UNAVAILABLE:
            txn->error.desc =
                "The server is unable to process the request at this time.";
            break;
        }
    }

    if (txn->error.desc) {
        const char **hdr, *host = "";
        char *port = NULL;
        unsigned level = 0;

        if (txn->req_hdrs &&
            (hdr = spool_getheader(txn->req_hdrs, ":authority")) &&
            hdr[0] && *hdr[0]) {
            host = (char *) hdr[0];
            if ((port = strchr(host, ':'))) *port++ = '\0';
        }
        else if (config_serverinfo != IMAP_ENUM_SERVERINFO_OFF) {
            host = config_servername;
        }
        if (!port) {
            port = (buf_len(&saslprops.iplocalport)) ?
                strchr(buf_cstring(&saslprops.iplocalport), ';')+1 : "";
        }

        buf_printf_markup(html, level, HTML_DOCTYPE);
        buf_printf_markup(html, level++, "<html>");
        buf_printf_markup(html, level++, "<head>");
        buf_printf_markup(html, level, "<title>%s</title>",
                          error_message(code));
        buf_printf_markup(html, --level, "</head>");
        buf_printf_markup(html, level++, "<body>");
        buf_printf_markup(html, level, "<h1>%s</h1>", error_message(code)+4);
        buf_printf_markup(html, level, "<p>%s</p>", txn->error.desc);
        buf_printf_markup(html, level, "<hr>");
        buf_printf_markup(html, level,
                          "<address>%s Server at %s Port %s</address>",
                          buf_cstring(&serverinfo), host, port);
        buf_printf_markup(html, --level, "</body>");
        buf_printf_markup(html, --level, "</html>");

        txn->resp_body.type = "text/html; charset=utf-8";
    }

    write_body(code, txn, buf_cstring(html), buf_len(html));
}
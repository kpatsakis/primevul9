EXPORTED void xml_response(long code, struct transaction_t *txn, xmlDocPtr xml)
{
    xmlChar *buf;
    int bufsiz;

    switch (code) {
    case HTTP_OK:
    case HTTP_CREATED:
    case HTTP_NO_CONTENT:
    case HTTP_MULTI_STATUS:
        break;

    default:
        /* Neither Brief nor Prefer affect error response bodies */
        txn->flags.vary &= ~(VARY_BRIEF | VARY_PREFER);
        txn->resp_body.prefs = 0;
    }

    /* Dump XML response tree into a text buffer */
    xmlDocDumpFormatMemoryEnc(xml, &buf, &bufsiz, "utf-8",
                              config_httpprettytelemetry);

    if (buf) {
        if (txn->flags.te & TE_CHUNKED) {
            /* Start of XML chunked response */
            xmlChar *cp;
            int n;

            /* Leave root element open */
            for (cp = buf + --bufsiz, n = 0; *cp != '/'; cp--, n++);
            if (*(cp+1) == '>') memmove(cp, cp+1, n);  /* <root/> */
            else bufsiz -= n+1;  /* </root> */
        }

        /* Output the XML response */
        txn->resp_body.type = "application/xml; charset=utf-8";

        write_body(code, txn, (char *) buf, bufsiz);

        /* Cleanup */
        xmlFree(buf);
    }
    else {
        txn->error.precond = 0;
        txn->error.desc = "Error dumping XML tree\r\n";
        error_response(HTTP_SERVER_ERROR, txn);
    }
}
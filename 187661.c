static int parse_request_line(struct transaction_t *txn)
{
    struct request_line_t *req_line = &txn->req_line;
    char *p;
    tok_t tok;
    int ret = 0;

    /* Trim CRLF from request-line */
    p = req_line->buf + strlen(req_line->buf);
    if (p[-1] == '\n') *--p = '\0';
    if (p[-1] == '\r') *--p = '\0';

    /* Parse request-line = method SP request-target SP HTTP-version CRLF */
    tok_initm(&tok, req_line->buf, " ", 0);
    if (!(req_line->meth = tok_next(&tok))) {
        ret = HTTP_BAD_REQUEST;
        txn->error.desc = "Missing method in request-line";
    }
    else if (!(req_line->uri = tok_next(&tok))) {
        ret = HTTP_BAD_REQUEST;
        txn->error.desc = "Missing request-target in request-line";
    }
    else if ((size_t) (p - req_line->buf) > MAX_REQ_LINE - 2) {
        /* request-line overran the size of our buffer */
        ret = HTTP_URI_TOO_LONG;
        buf_printf(&txn->buf,
                   "Length of request-line MUST be less than %u octets",
                   MAX_REQ_LINE);
        txn->error.desc = buf_cstring(&txn->buf);
    }
    else if (!(req_line->ver = tok_next(&tok))) {
        ret = HTTP_BAD_REQUEST;
        txn->error.desc = "Missing HTTP-version in request-line";
    }
    else if (tok_next(&tok)) {
        ret = HTTP_BAD_REQUEST;
        txn->error.desc = "Unexpected extra argument(s) in request-line";
    }

    /* Check HTTP-Version - MUST be HTTP/1.x */
    else if (strlen(req_line->ver) != HTTP_VERSION_LEN
             || strncmp(req_line->ver, HTTP_VERSION, HTTP_VERSION_LEN-1)
             || !isdigit(req_line->ver[HTTP_VERSION_LEN-1])) {
        ret = HTTP_BAD_VERSION;
        buf_printf(&txn->buf,
                   "This server only speaks %.*sx",
                   HTTP_VERSION_LEN-1, HTTP_VERSION);
        txn->error.desc = buf_cstring(&txn->buf);
    }
    else if (req_line->ver[HTTP_VERSION_LEN-1] == '0') {
        /* HTTP/1.0 connection */
        txn->flags.ver = VER_1_0;
    }
    tok_fini(&tok);

    return ret;
}
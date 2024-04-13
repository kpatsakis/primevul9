EXPORTED int http_read_req_body(struct transaction_t *txn)
{
    struct body_t *body = &txn->req_body;

    syslog(LOG_DEBUG, "http_read_req_body(flags=%#x, framing=%d)",
           body->flags, body->framing);

    if (body->flags & BODY_DONE) return 0;
    body->flags |= BODY_DONE;

    if (body->flags & BODY_CONTINUE) {
        body->flags &= ~BODY_CONTINUE;

        if (body->flags & BODY_DISCARD) {
            /* Don't care about the body and client hasn't sent it, we're done */
            return 0;
        }

        /* Tell client to send the body */
        response_header(HTTP_CONTINUE, txn);
    }

    /* Read body from client */
    return http_read_body(txn->conn->pin, txn->req_hdrs, body, &txn->error.desc);
}
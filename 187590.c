static void transaction_reset(struct transaction_t *txn)
{
    txn->meth = METH_UNKNOWN;

    memset(&txn->flags, 0, sizeof(struct txn_flags_t));
    txn->flags.ver = VER_1_1;
    txn->flags.vary = VARY_AE;

    memset(&txn->req_line, 0, sizeof(struct request_line_t));

    /* Reset Bearer auth scheme for each transaction */
    avail_auth_schemes &= ~AUTH_BEARER;

    if (txn->req_uri) xmlFreeURI(txn->req_uri);
    txn->req_uri = NULL;

    /* XXX - split this into a req_tgt cleanup */
    free(txn->req_tgt.userid);
    mboxlist_entry_free(&txn->req_tgt.mbentry);
    memset(&txn->req_tgt, 0, sizeof(struct request_target_t));

    free_hash_table(&txn->req_qparams, (void (*)(void *)) &freestrlist);

    if (txn->req_hdrs) spool_free_hdrcache(txn->req_hdrs);
    txn->req_hdrs = NULL;

    txn->req_body.flags = 0;
    buf_reset(&txn->req_body.payload);

    txn->auth_chal.param = NULL;
    txn->location = NULL;
    memset(&txn->error, 0, sizeof(struct error_t));

    strarray_fini(&txn->resp_body.links);
    memset(&txn->resp_body, 0,  /* Don't zero the response payload buffer */
           sizeof(struct resp_body_t) - sizeof(struct buf));
    buf_reset(&txn->resp_body.payload);

    /* Pre-allocate our working buffer */
    buf_reset(&txn->buf);
    buf_ensure(&txn->buf, 1024);
}
void parse_query_params(struct transaction_t *txn, const char *query)
{
    tok_t tok;
    char *param;

    assert(!buf_len(&txn->buf));  /* Unescape buffer */

    tok_init(&tok, query, "&", TOK_TRIMLEFT|TOK_TRIMRIGHT|TOK_EMPTY);
    while ((param = tok_next(&tok))) {
        struct strlist *vals;
        char *key, *value;
        size_t len;

        /* Split param into key and optional value */
        key = param;
        value = strchr(param, '=');

        if (!value) value = "";
        else *value++ = '\0';
        len = strlen(value);
        buf_ensure(&txn->buf, len+1);

        vals = hash_lookup(key, &txn->req_qparams);
        appendstrlist(&vals, xmlURIUnescapeString(value, len, txn->buf.s));
        hash_insert(key, vals, &txn->req_qparams);
    }
    tok_fini(&tok);

    buf_reset(&txn->buf);
}
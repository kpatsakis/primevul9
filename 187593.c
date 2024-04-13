static int parse_expect(struct transaction_t *txn)
{
    const char **exp = spool_getheader(txn->req_hdrs, "Expect");
    int i, ret = 0;

    /* Expect not supported by HTTP/1.0 clients */
    if (exp && txn->flags.ver == VER_1_0) return HTTP_EXPECT_FAILED;

    /* Look for interesting expectations.  Unknown == error */
    for (i = 0; !ret && exp && exp[i]; i++) {
        tok_t tok = TOK_INITIALIZER(exp[i], ",", TOK_TRIMLEFT|TOK_TRIMRIGHT);
        char *token;

        while (!ret && (token = tok_next(&tok))) {
            /* Check if client wants acknowledgment before sending body */ 
            if (!strcasecmp(token, "100-continue")) {
                syslog(LOG_DEBUG, "Expect: 100-continue");
                txn->req_body.flags |= BODY_CONTINUE;
            }
            else {
                txn->error.desc = "Unsupported Expectation";
                ret = HTTP_EXPECT_FAILED;
            }
        }

        tok_fini(&tok);
    }

    return ret;
}
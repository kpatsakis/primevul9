static int starttls(struct transaction_t *txn __attribute__((unused)),
                    struct http_connection *conn __attribute__((unused)))
{
    fatal("starttls() called, but no OpenSSL", EX_SOFTWARE);
}
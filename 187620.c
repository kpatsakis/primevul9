static int zstd_compress(struct transaction_t *txn __attribute__((unused)),
                           unsigned flags __attribute__((unused)),
                           const char *buf __attribute__((unused)),
                           unsigned len __attribute__((unused)))
{
    fatal("Zstandard Compression requested, but not available", EX_SOFTWARE);
}
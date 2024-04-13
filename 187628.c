static void write_cachehdr(const char *name, const char *contents,
                           const char *raw __attribute__((unused)), void *rock)
{
    struct transaction_t *txn = (struct transaction_t *) rock;
    const char **hdr, *hop_by_hop[] =
        { "connection", "content-length", "content-type", "date", "forwarded",
          "keep-alive", "location", "status", "strict-transport-security",
          "upgrade", "via", NULL };

    /* Ignore private headers in our cache */
    if (name[0] == ':') return;

    for (hdr = hop_by_hop; *hdr; hdr++) {
        if (!strcasecmp(name, *hdr)) return;
    }

    simple_hdr(txn, name, contents);
}
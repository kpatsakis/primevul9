static int listvalidator(void *ic, const char *list)
{
    struct sieve_interp_ctx *ctx = (struct sieve_interp_ctx *) ic;
    char *abook = get_addrbook_mboxname(list, ctx->userid);
    int ret = abook ? SIEVE_OK : SIEVE_FAIL;

    free(abook);

    return ret;
}
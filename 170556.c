static int listcompare(const char *text, size_t tlen __attribute__((unused)),
                       const char *list, strarray_t *match_vars, void *rock)
{
    struct sieve_interp_ctx *ctx = (struct sieve_interp_ctx *) rock;
    char *abook = get_addrbook_mboxname(list, ctx->userid);
    int ret = 0;

    if (!abook) return 0;

    if (!ctx->carddavdb) {
        /* open user's CardDAV DB */
        ctx->carddavdb = carddav_open_userid(ctx->userid);
    }
    if (ctx->carddavdb) {
        /* search for email address in addressbook */
        strarray_t *uids =
            carddav_getemail2details(ctx->carddavdb, text, abook, NULL);
        ret = strarray_size(uids);

        strarray_free(uids);
    }

    if (ret && match_vars) {
        /* found a match - set $0 */
        strarray_add(match_vars, text);
    }

    free(abook);

    return ret;
}
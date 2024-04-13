yang_read_pattern(struct ly_ctx *ctx, struct lys_restr *pattern, void **precomp, char *value, char modifier)
{
    char *buf;
    size_t len;

    if (precomp && lyp_precompile_pattern(ctx, value, (pcre**)&precomp[0], (pcre_extra**)&precomp[1])) {
        free(value);
        return EXIT_FAILURE;
    }

    len = strlen(value);
    buf = malloc((len + 2) * sizeof *buf); /* modifier byte + value + terminating NULL byte */
    LY_CHECK_ERR_RETURN(!buf, LOGMEM(ctx); free(value), EXIT_FAILURE);

    buf[0] = modifier;
    strcpy(&buf[1], value);
    free(value);

    pattern->expr = lydict_insert_zc(ctx, buf);
    return EXIT_SUCCESS;
}
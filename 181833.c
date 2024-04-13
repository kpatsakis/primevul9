lyp_precompile_pattern(struct ly_ctx *ctx, const char *pattern, pcre** pcre_cmp, pcre_extra **pcre_std)
{
    const char *err_msg = NULL;

    if (lyp_check_pattern(ctx, pattern, pcre_cmp)) {
        return EXIT_FAILURE;
    }

    if (pcre_std && pcre_cmp) {
        (*pcre_std) = pcre_study(*pcre_cmp, 0, &err_msg);
        if (err_msg) {
            LOGWRN(ctx, "Studying pattern \"%s\" failed (%s).", pattern, err_msg);
        }
    }

    return EXIT_SUCCESS;
}
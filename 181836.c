validate_pattern(struct ly_ctx *ctx, const char *val_str, struct lys_type *type, struct lyd_node *node)
{
    int rc;
    unsigned int i;
#ifndef LY_ENABLED_CACHE
    pcre *precomp;
#endif

    assert(ctx && (type->base == LY_TYPE_STRING));

    if (!val_str) {
        val_str = "";
    }

    if (type->der && validate_pattern(ctx, val_str, &type->der->type, node)) {
        return EXIT_FAILURE;
    }

#ifdef LY_ENABLED_CACHE
    /* there is no cache, build it */
    if (!type->info.str.patterns_pcre && type->info.str.pat_count) {
        type->info.str.patterns_pcre = malloc(2 * type->info.str.pat_count * sizeof *type->info.str.patterns_pcre);
        LY_CHECK_ERR_RETURN(!type->info.str.patterns_pcre, LOGMEM(ctx), -1);

        for (i = 0; i < type->info.str.pat_count; ++i) {
            if (lyp_precompile_pattern(ctx, &type->info.str.patterns[i].expr[1],
                                       (pcre**)&type->info.str.patterns_pcre[i * 2],
                                       (pcre_extra**)&type->info.str.patterns_pcre[i * 2 + 1])) {
                return EXIT_FAILURE;
            }
        }
    }
#endif

    for (i = 0; i < type->info.str.pat_count; ++i) {
#ifdef LY_ENABLED_CACHE
        rc = pcre_exec((pcre *)type->info.str.patterns_pcre[2 * i], (pcre_extra *)type->info.str.patterns_pcre[2 * i + 1],
                       val_str, strlen(val_str), 0, 0, NULL, 0);
#else
        if (lyp_check_pattern(ctx, &type->info.str.patterns[i].expr[1], &precomp)) {
            return EXIT_FAILURE;
        }
        rc = pcre_exec(precomp, NULL, val_str, strlen(val_str), 0, 0, NULL, 0);
        free(precomp);
#endif
        if ((rc && type->info.str.patterns[i].expr[0] == 0x06) || (!rc && type->info.str.patterns[i].expr[0] == 0x15)) {
            LOGVAL(ctx, LYE_NOCONSTR, LY_VLOG_LYD, node, val_str, &type->info.str.patterns[i].expr[1]);
            if (type->info.str.patterns[i].emsg) {
                ly_vlog_str(ctx, LY_VLOG_PREV, type->info.str.patterns[i].emsg);
            }
            if (type->info.str.patterns[i].eapptag) {
                ly_err_last_set_apptag(ctx, type->info.str.patterns[i].eapptag);
            }
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}
yang_fill_extcomplex_module(struct ly_ctx *ctx, struct lys_ext_instance_complex *ext,
                            char *parent_name, char **values, int implemented)
{
    int c, i;
    struct lys_module **modules, ***p, *reallocated, **pp;
    struct lyext_substmt *info;

    if (!values) {
        return EXIT_SUCCESS;
    }
    pp = modules = lys_ext_complex_get_substmt(LY_STMT_MODULE, ext, &info);
    if (!modules) {
        LOGVAL(ctx, LYE_INCHILDSTMT, LY_VLOG_NONE, NULL, "module", parent_name);
        return EXIT_FAILURE;
    }

    for (i = 0; values[i]; ++i) {
        c = 0;
        if (info->cardinality < LY_STMT_CARD_SOME && *modules) {
            LOGVAL(ctx, LYE_TOOMANY, LY_VLOG_NONE, NULL, "module", parent_name);
            return EXIT_FAILURE;
        }
        if (info->cardinality >= LY_STMT_CARD_SOME) {
            /* there can be multiple instances, so instead of pointer to array,
             * we have in modules pointer to pointer to array */
            p = (struct lys_module ***)pp;
            modules = *p;
            if (!modules) {
                /* allocate initial array */
                *p = modules = calloc(2, sizeof(struct lys_module *));
                LY_CHECK_ERR_RETURN(!*p, LOGMEM(ctx), EXIT_FAILURE);
            } else {
                for (c = 0; *modules; modules++, c++);
            }
        }

        if (c) {
            /* enlarge the array */
            reallocated = realloc(*p, (c + 2) * sizeof(struct lys_module *));
            LY_CHECK_ERR_RETURN(!reallocated, LOGMEM(ctx), EXIT_FAILURE);
            *p = (struct lys_module **)reallocated;
            modules = *p;
            modules[c + 1] = NULL;
        }

        modules[c] = yang_read_module(ctx, values[i], 0, NULL, implemented);
        if (!modules[c]) {
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}
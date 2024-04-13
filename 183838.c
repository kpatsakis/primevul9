yang_fill_extcomplex_uint8(struct lys_ext_instance_complex *ext, char *parent_name, char *node_name,
                           LY_STMT stmt, uint8_t value)
{
    struct ly_ctx *ctx = ext->module->ctx;
    uint8_t *val, **pp = NULL, *reallocated;
    struct lyext_substmt *info;
    int i = 0;

    val = lys_ext_complex_get_substmt(stmt, ext, &info);
    if (!val) {
        LOGVAL(ctx, LYE_INCHILDSTMT, LY_VLOG_NONE, NULL, node_name, parent_name);
        return EXIT_FAILURE;
    }
    if (stmt == LY_STMT_DIGITS) {
        if (info->cardinality < LY_STMT_CARD_SOME && *val) {
            LOGVAL(ctx, LYE_TOOMANY, LY_VLOG_NONE, NULL, node_name, parent_name);
            return EXIT_FAILURE;
        }

        if (info->cardinality >= LY_STMT_CARD_SOME) {
            /* there can be multiple instances */
            pp = (uint8_t**)val;
            if (!(*pp)) {
                *pp = calloc(2, sizeof(uint8_t)); /* allocate initial array */
                LY_CHECK_ERR_RETURN(!*pp, LOGMEM(ctx), EXIT_FAILURE);
            } else {
                for (i = 0; (*pp)[i]; i++);
            }
            val = &(*pp)[i];
        }

        /* stored value */
        *val = value;

        if (i) {
            /* enlarge the array */
            reallocated = realloc(*pp, (i + 2) * sizeof *val);
            LY_CHECK_ERR_RETURN(!reallocated, LOGMEM(ctx), EXIT_FAILURE);
            *pp = reallocated;
            (*pp)[i + 1] = 0;
        }
    } else {
        if (*val) {
            LOGVAL(ctx, LYE_TOOMANY, LY_VLOG_NONE, NULL, node_name, parent_name);
            return EXIT_FAILURE;
        }

        if (stmt == LY_STMT_REQINSTANCE) {
            *val = (value == 1) ? 1 : 2;
        } else if (stmt == LY_STMT_MODIFIER) {
            *val =  1;
        } else {
            LOGINT(ctx);
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}
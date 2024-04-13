yang_getplace_for_extcomplex_struct(struct lys_ext_instance_complex *ext, int *index,
                                    char *parent_name, char *node_name, LY_STMT stmt)
{
    struct ly_ctx *ctx = ext->module->ctx;
    int c;
    void **data, ***p = NULL;
    void *reallocated;
    struct lyext_substmt *info;

    data = lys_ext_complex_get_substmt(stmt, ext, &info);
    if (!data) {
        LOGVAL(ctx, LYE_INCHILDSTMT, LY_VLOG_NONE, NULL, node_name, parent_name);
        return NULL;
    }
    if (info->cardinality < LY_STMT_CARD_SOME && *data) {
        LOGVAL(ctx, LYE_TOOMANY, LY_VLOG_NONE, NULL, node_name, parent_name);
        return NULL;
    }

    c = 0;
    if (info->cardinality >= LY_STMT_CARD_SOME) {
        /* there can be multiple instances, so instead of pointer to array,
         * we have in data pointer to pointer to array */
        p = (void ***)data;
        data = *p;
        if (!data) {
            /* allocate initial array */
            *p = data = calloc(2, sizeof(void *));
            LY_CHECK_ERR_RETURN(!data, LOGMEM(ctx), NULL);
        } else {
            for (c = 0; *data; data++, c++);
        }
    }

    if (c) {
        /* enlarge the array */
        reallocated = realloc(*p, (c + 2) * sizeof(void *));
        LY_CHECK_ERR_RETURN(!reallocated, LOGMEM(ctx), NULL);
        *p = reallocated;
        data = *p;
        data[c + 1] = NULL;
    }

    if (index) {
        *index = c;
        return data;
    } else {
        return &data[c];
    }
}
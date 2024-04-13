validate_length_range(uint8_t kind, uint64_t unum, int64_t snum, int64_t fnum, uint8_t fnum_dig, struct lys_type *type,
                      const char *val_str, struct lyd_node *node)
{
    struct lys_restr *restr = NULL;
    struct len_ran_intv *intv = NULL, *tmp_intv;
    struct lys_type *cur_type;
    struct ly_ctx *ctx = type->parent->module->ctx;
    int match;

    if (resolve_len_ran_interval(ctx, NULL, type, &intv)) {
        /* already done during schema parsing */
        LOGINT(ctx);
        return EXIT_FAILURE;
    }
    if (!intv) {
        return EXIT_SUCCESS;
    }

    /* I know that all intervals belonging to a single restriction share one type pointer */
    tmp_intv = intv;
    cur_type = intv->type;
    do {
        match = 0;
        for (; tmp_intv && (tmp_intv->type == cur_type); tmp_intv = tmp_intv->next) {
            if (match) {
                /* just iterate through the rest of this restriction intervals */
                continue;
            }

            if (((kind == 0) && (unum < tmp_intv->value.uval.min))
                    || ((kind == 1) && (snum < tmp_intv->value.sval.min))
                    || ((kind == 2) && (dec64cmp(fnum, fnum_dig, tmp_intv->value.fval.min, cur_type->info.dec64.dig) < 0))) {
                break;
            }

            if (((kind == 0) && (unum >= tmp_intv->value.uval.min) && (unum <= tmp_intv->value.uval.max))
                    || ((kind == 1) && (snum >= tmp_intv->value.sval.min) && (snum <= tmp_intv->value.sval.max))
                    || ((kind == 2) && (dec64cmp(fnum, fnum_dig, tmp_intv->value.fval.min, cur_type->info.dec64.dig) > -1)
                    && (dec64cmp(fnum, fnum_dig, tmp_intv->value.fval.max, cur_type->info.dec64.dig) < 1))) {
                match = 1;
            }
        }

        if (!match) {
            break;
        } else if (tmp_intv) {
            cur_type = tmp_intv->type;
        }
    } while (tmp_intv);

    while (intv) {
        tmp_intv = intv->next;
        free(intv);
        intv = tmp_intv;
    }

    if (!match) {
        switch (cur_type->base) {
        case LY_TYPE_BINARY:
            restr = cur_type->info.binary.length;
            break;
        case LY_TYPE_DEC64:
            restr = cur_type->info.dec64.range;
            break;
        case LY_TYPE_INT8:
        case LY_TYPE_INT16:
        case LY_TYPE_INT32:
        case LY_TYPE_INT64:
        case LY_TYPE_UINT8:
        case LY_TYPE_UINT16:
        case LY_TYPE_UINT32:
        case LY_TYPE_UINT64:
            restr = cur_type->info.num.range;
            break;
        case LY_TYPE_STRING:
            restr = cur_type->info.str.length;
            break;
        default:
            LOGINT(ctx);
            return EXIT_FAILURE;
        }

        LOGVAL(ctx, LYE_NOCONSTR, LY_VLOG_LYD, node, (val_str ? val_str : ""), restr ? restr->expr : "");
        if (restr && restr->emsg) {
            ly_vlog_str(ctx, LY_VLOG_PREV, restr->emsg);
        }
        if (restr && restr->eapptag) {
            ly_err_last_set_apptag(ctx, restr->eapptag);
        }
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
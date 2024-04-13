yang_read_range(struct ly_ctx *ctx, struct yang_type *stype, char *value, int is_ext_instance)
{
    struct lys_restr * range;

    if (is_ext_instance) {
        range = (struct lys_restr *)stype;
    } else {
        if (stype->base != 0 && stype->base != LY_TYPE_DEC64) {
            LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "Unexpected range statement.");
            goto error;
        }
        stype->base = LY_TYPE_DEC64;
        if (stype->type->info.dec64.range) {
            LOGVAL(ctx, LYE_TOOMANY, LY_VLOG_NONE, NULL, "range", "type");
            goto error;
        }
        range = calloc(1, sizeof *range);
        LY_CHECK_ERR_GOTO(!range, LOGMEM(ctx), error);
        stype->type->info.dec64.range = range;
    }
    range->expr = lydict_insert_zc(ctx, value);
    return range;

error:
    free(value);
    return NULL;
}
yang_read_length(struct ly_ctx *ctx, struct yang_type *stype, char *value, int is_ext_instance)
{
    struct lys_restr *length;

    if (is_ext_instance) {
        length = (struct lys_restr *)stype;
    } else {
        if (stype->base == 0 || stype->base == LY_TYPE_STRING) {
            stype->base = LY_TYPE_STRING;
        } else {
            LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "Unexpected length statement.");
            goto error;
        }

        if (stype->type->info.str.length) {
            LOGVAL(ctx, LYE_TOOMANY, LY_VLOG_NONE, NULL, "length", "type");
            goto error;
        }
        length = calloc(1, sizeof *length);
        LY_CHECK_ERR_GOTO(!length, LOGMEM(ctx), error);
        stype->type->info.str.length = length;
    }
    length->expr = lydict_insert_zc(ctx, value);
    return length;

error:
    free(value);
    return NULL;
}
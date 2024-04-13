yang_read_fraction(struct ly_ctx *ctx, struct yang_type *typ, uint32_t value)
{
    uint32_t i;

    if (typ->base == 0 || typ->base == LY_TYPE_DEC64) {
        typ->base = LY_TYPE_DEC64;
    } else {
        LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "Unexpected fraction-digits statement.");
        goto error;
    }
    if (typ->type->info.dec64.dig) {
        LOGVAL(ctx, LYE_TOOMANY, LY_VLOG_NONE, NULL, "fraction-digits", "type");
        goto error;
    }
    /* range check */
    if (value < 1 || value > 18) {
        LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "Invalid value \"%d\" of \"%s\".", value, "fraction-digits");
        goto error;
    }
    typ->type->info.dec64.dig = value;
    typ->type->info.dec64.div = 10;
    for (i = 1; i < value; i++) {
        typ->type->info.dec64.div *= 10;
    }
    return EXIT_SUCCESS;

error:
    return EXIT_FAILURE;
}
yang_read_bit(struct ly_ctx *ctx, struct yang_type *typ, struct lys_type_bit *bit, char *value)
{
    int i, j;

    typ->base = LY_TYPE_BITS;
    bit->name = lydict_insert_zc(ctx, value);
    if (lyp_check_identifier(ctx, bit->name, LY_IDENT_SIMPLE, NULL, NULL)) {
        goto error;
    }

    j = typ->type->info.bits.count - 1;
    /* check the name uniqueness */
    for (i = 0; i < j; i++) {
        if (ly_strequal(typ->type->info.bits.bit[i].name, bit->name, 1)) {
            LOGVAL(ctx, LYE_BITS_DUPNAME, LY_VLOG_NONE, NULL, bit->name);
            goto error;
        }
    }
    return EXIT_SUCCESS;

error:
    return EXIT_FAILURE;
}
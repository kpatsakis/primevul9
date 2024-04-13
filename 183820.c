yang_type_free(struct ly_ctx *ctx, struct lys_type *type)
{
    struct yang_type *stype = (struct yang_type *)type->der;
    unsigned int i;

    if (!stype) {
        return ;
    }
    if (type->base == LY_TYPE_DER || type->base == LY_TYPE_UNION) {
        lydict_remove(ctx, stype->name);
        if (stype->base == LY_TYPE_IDENT && (!(stype->flags & LYS_NO_ERASE_IDENTITY))) {
            for (i = 0; i < type->info.ident.count; ++i) {
                free(type->info.ident.ref[i]);
            }
        }
        if (stype->base == LY_TYPE_UNION) {
            for (i = 0; i < type->info.uni.count; ++i) {
                yang_type_free(ctx, &type->info.uni.types[i]);
            }
            free(type->info.uni.types);
            type->base = LY_TYPE_DER;
        } else {
            type->base = stype->base;
        }
        free(stype);
        type->der = NULL;
    }
    lys_type_free(ctx, type, NULL);
    memset(type, 0, sizeof (struct lys_type));
}
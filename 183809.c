yang_free_type_union(struct ly_ctx *ctx, struct lys_type *type)
{
    struct lys_type *stype;
    struct yang_type *yang;
    unsigned int i;

    for (i = 0; i < type->info.uni.count; ++i) {
        stype = &type->info.uni.types[i];
        if (stype->base == LY_TYPE_DER) {
            yang = (struct yang_type *)stype->der;
            stype->base = yang->base;
            lydict_remove(ctx, yang->name);
            free(yang);
        } else if (stype->base == LY_TYPE_UNION) {
            yang_free_type_union(ctx, stype);
        }
    }
}
yang_tpdf_free(struct ly_ctx *ctx, struct lys_tpdf *tpdf, uint16_t start, uint16_t size)
{
    uint8_t i;

    assert(ctx);
    if (!tpdf) {
        return;
    }

    for (i = start; i < size; ++i) {
        lydict_remove(ctx, tpdf[i].name);
        lydict_remove(ctx, tpdf[i].dsc);
        lydict_remove(ctx, tpdf[i].ref);

        yang_type_free(ctx, &tpdf[i].type);

        lydict_remove(ctx, tpdf[i].units);
        lydict_remove(ctx, tpdf[i].dflt);
        lys_extension_instances_free(ctx, tpdf[i].ext, tpdf[i].ext_size, NULL);
    }
}
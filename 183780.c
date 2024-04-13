yang_free_container(struct ly_ctx *ctx, struct lys_node_container * cont)
{
    uint8_t i;

    yang_tpdf_free(ctx, cont->tpdf, 0, cont->tpdf_size);
    free(cont->tpdf);
    lydict_remove(ctx, cont->presence);

    for (i = 0; i < cont->must_size; ++i) {
        lys_restr_free(ctx, &cont->must[i], NULL);
    }
    free(cont->must);

    lys_when_free(ctx, cont->when, NULL);
}
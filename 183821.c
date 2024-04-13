yang_free_inout(struct ly_ctx *ctx, struct lys_node_inout *inout)
{
    uint8_t i;

    yang_tpdf_free(ctx, inout->tpdf, 0, inout->tpdf_size);
    free(inout->tpdf);

    for (i = 0; i < inout->must_size; ++i) {
        lys_restr_free(ctx, &inout->must[i], NULL);
    }
    free(inout->must);
}
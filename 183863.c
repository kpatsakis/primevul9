yang_free_anydata(struct ly_ctx *ctx, struct lys_node_anydata *anydata)
{
    uint8_t i;

    for (i = 0; i < anydata->must_size; ++i) {
        lys_restr_free(ctx, &anydata->must[i], NULL);
    }
    free(anydata->must);

    lys_when_free(ctx, anydata->when, NULL);
}
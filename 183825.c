yang_free_list(struct ly_ctx *ctx, struct lys_node_list *list)
{
    uint8_t i;

    yang_tpdf_free(ctx, list->tpdf, 0, list->tpdf_size);
    free(list->tpdf);

    for (i = 0; i < list->must_size; ++i) {
        lys_restr_free(ctx, &list->must[i], NULL);
    }
    free(list->must);

    lys_when_free(ctx, list->when, NULL);

    for (i = 0; i < list->unique_size; ++i) {
        free(list->unique[i].expr);
    }
    free(list->unique);

    free(list->keys);
}
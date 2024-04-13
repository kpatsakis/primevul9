yang_free_notif(struct ly_ctx *ctx, struct lys_node_notif *notif)
{
    uint8_t i;

    yang_tpdf_free(ctx, notif->tpdf, 0, notif->tpdf_size);
    free(notif->tpdf);

    for (i = 0; i < notif->must_size; ++i) {
        lys_restr_free(ctx, &notif->must[i], NULL);
    }
    free(notif->must);
}
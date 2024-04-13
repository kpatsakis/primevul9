yang_free_leaf(struct ly_ctx *ctx, struct lys_node_leaf *leaf)
{
    uint8_t i;

    for (i = 0; i < leaf->must_size; i++) {
        lys_restr_free(ctx, &leaf->must[i], NULL);
    }
    free(leaf->must);

    lys_when_free(ctx, leaf->when, NULL);

    yang_type_free(ctx, &leaf->type);
    lydict_remove(ctx, leaf->units);
    lydict_remove(ctx, leaf->dflt);
}
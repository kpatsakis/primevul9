yang_free_leaflist(struct ly_ctx *ctx, struct lys_node_leaflist *leaflist)
{
    uint8_t i;

    for (i = 0; i < leaflist->must_size; i++) {
        lys_restr_free(ctx, &leaflist->must[i], NULL);
    }
    free(leaflist->must);

    for (i = 0; i < leaflist->dflt_size; i++) {
        lydict_remove(ctx, leaflist->dflt[i]);
    }
    free(leaflist->dflt);

    lys_when_free(ctx, leaflist->when, NULL);

    yang_type_free(ctx, &leaflist->type);
    lydict_remove(ctx, leaflist->units);
}
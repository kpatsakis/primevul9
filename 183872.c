yang_free_grouping(struct ly_ctx *ctx, struct lys_node_grp * grp)
{
    yang_tpdf_free(ctx, grp->tpdf, 0, grp->tpdf_size);
    free(grp->tpdf);
}
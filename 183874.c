yang_free_choice(struct ly_ctx *ctx, struct lys_node_choice *choice)
{
    free(choice->dflt);
    lys_when_free(ctx, choice->when, NULL);
}
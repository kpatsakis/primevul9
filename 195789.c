int32_t cli_bcapi_hashset_empty(struct cli_bc_ctx *ctx, int32_t id)
{
    struct cli_hashset *s = get_hashset(ctx, id);
    return !s->count;
}
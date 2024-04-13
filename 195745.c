static struct cli_map *get_hashtab(struct cli_bc_ctx *ctx, int32_t id)
{
    if (id < 0 || id >= ctx->nmaps || !ctx->maps)
	return NULL;
    return &ctx->maps[id];
}
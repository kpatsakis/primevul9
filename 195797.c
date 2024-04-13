static struct bc_jsnorm *get_jsnorm(struct cli_bc_ctx *ctx, int32_t id)
{
    if (id < 0 || id >= ctx->njsnorms || !ctx->jsnorms)
	return NULL;
    return &ctx->jsnorms[id];
}
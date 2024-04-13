static struct bc_inflate *get_inflate(struct cli_bc_ctx *ctx, int32_t id)
{
    if (id < 0 || id >= ctx->ninflates || !ctx->inflates)
	return NULL;
    return &ctx->inflates[id];
}
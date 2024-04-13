static struct cli_hashset *get_hashset(struct cli_bc_ctx *ctx, int32_t id)
{
    if (id < 0 || id >= ctx->nhashsets || !ctx->hashsets) {
	API_MISUSE();
	return NULL;
    }
    return &ctx->hashsets[id];
}
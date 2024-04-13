int32_t cli_bcapi_map_new(struct cli_bc_ctx *ctx, int32_t keysize, int32_t valuesize)
{
    unsigned n = ctx->nmaps+1;
    struct cli_map *s;
    if (!keysize)
	return -1;
    s = cli_realloc(ctx->maps, sizeof(*ctx->maps)*n);
    if (!s)
	return -1;
    ctx->maps = s;
    ctx->nmaps = n;
    s = &s[n-1];
    cli_map_init(s, keysize, valuesize, 16);
    return n-1;
}
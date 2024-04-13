int32_t cli_bcapi_map_done(struct cli_bc_ctx *ctx , int32_t id)
{
    struct cli_map *s = get_hashtab(ctx, id);
    if (!s)
	return -1;
    cli_map_delete(s);
    if (id == ctx->nmaps-1) {
	ctx->nmaps--;
	if (!ctx->nmaps) {
	    free(ctx->maps);
	    ctx->maps = NULL;
	} else {
	    s = cli_realloc(ctx->maps, ctx->nmaps*(sizeof(*s)));
	    if (s)
		ctx->maps = s;
	}
    }
    return 0;
}
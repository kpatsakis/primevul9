int32_t cli_bcapi_hashset_done(struct cli_bc_ctx *ctx , int32_t id)
{
    struct cli_hashset *s = get_hashset(ctx, id);
    if (!s)
	return -1;
    cli_hashset_destroy(s);
    if (id == ctx->nhashsets-1) {
	ctx->nhashsets--;
	if (!ctx->nhashsets) {
	    free(ctx->hashsets);
	    ctx->hashsets = NULL;
	} else {
	    s = cli_realloc(ctx->hashsets, ctx->nhashsets*sizeof(*s));
	    if (s)
		ctx->hashsets = s;
	}
    }
    return 0;
}
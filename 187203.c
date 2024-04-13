static void nfs4_close_context(struct nfs_open_context *ctx, int is_sync)
{
	if (ctx->state == NULL)
		return;
	if (is_sync)
		nfs4_close_sync(ctx->state, ctx->mode);
	else
		nfs4_close_state(ctx->state, ctx->mode);
}
static int nfs4_open_reclaim(struct nfs4_state_owner *sp, struct nfs4_state *state)
{
	struct nfs_open_context *ctx;
	int ret;

	ctx = nfs4_state_find_open_context(state);
	if (IS_ERR(ctx))
		return PTR_ERR(ctx);
	ret = nfs4_do_open_reclaim(ctx, state);
	put_nfs_open_context(ctx);
	return ret;
}
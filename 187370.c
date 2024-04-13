nfs4_atomic_open(struct inode *dir, struct nfs_open_context *ctx, int open_flags, struct iattr *attr)
{
	struct nfs4_state *state;

	/* Protect against concurrent sillydeletes */
	state = nfs4_do_open(dir, ctx->dentry, ctx->mode, open_flags, attr,
			     ctx->cred, &ctx->mdsthreshold);
	if (IS_ERR(state))
		return ERR_CAST(state);
	ctx->state = state;
	return igrab(state->inode);
}
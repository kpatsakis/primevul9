nfs4_proc_create(struct inode *dir, struct dentry *dentry, struct iattr *sattr,
		 int flags)
{
	struct nfs_open_context *ctx;
	struct nfs4_state *state;
	int status = 0;

	ctx = alloc_nfs_open_context(dentry, FMODE_READ);
	if (IS_ERR(ctx))
		return PTR_ERR(ctx);

	sattr->ia_mode &= ~current_umask();
	state = nfs4_do_open(dir, dentry, ctx->mode,
			flags, sattr, ctx->cred,
			&ctx->mdsthreshold);
	d_drop(dentry);
	if (IS_ERR(state)) {
		status = PTR_ERR(state);
		goto out;
	}
	d_add(dentry, igrab(state->inode));
	nfs_set_verifier(dentry, nfs_save_change_attribute(dir));
	ctx->state = state;
out:
	put_nfs_open_context(ctx);
	return status;
}
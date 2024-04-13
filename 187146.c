static struct nfs_open_context *nfs4_state_find_open_context(struct nfs4_state *state)
{
	struct nfs_inode *nfsi = NFS_I(state->inode);
	struct nfs_open_context *ctx;

	spin_lock(&state->inode->i_lock);
	list_for_each_entry(ctx, &nfsi->open_files, list) {
		if (ctx->state != state)
			continue;
		get_nfs_open_context(ctx);
		spin_unlock(&state->inode->i_lock);
		return ctx;
	}
	spin_unlock(&state->inode->i_lock);
	return ERR_PTR(-ENOENT);
}
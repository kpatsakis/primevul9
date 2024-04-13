nfs4_proc_setattr(struct dentry *dentry, struct nfs_fattr *fattr,
		  struct iattr *sattr)
{
	struct inode *inode = dentry->d_inode;
	struct rpc_cred *cred = NULL;
	struct nfs4_state *state = NULL;
	int status;

	if (pnfs_ld_layoutret_on_setattr(inode))
		pnfs_return_layout(inode);

	nfs_fattr_init(fattr);
	
	/* Deal with open(O_TRUNC) */
	if (sattr->ia_valid & ATTR_OPEN)
		sattr->ia_valid &= ~(ATTR_MTIME|ATTR_CTIME|ATTR_OPEN);

	/* Optimization: if the end result is no change, don't RPC */
	if ((sattr->ia_valid & ~(ATTR_FILE)) == 0)
		return 0;

	/* Search for an existing open(O_WRITE) file */
	if (sattr->ia_valid & ATTR_FILE) {
		struct nfs_open_context *ctx;

		ctx = nfs_file_open_context(sattr->ia_file);
		if (ctx) {
			cred = ctx->cred;
			state = ctx->state;
		}
	}

	status = nfs4_do_setattr(inode, cred, fattr, sattr, state);
	if (status == 0)
		nfs_setattr_update_inode(inode, sattr);
	return status;
}
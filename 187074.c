static int _nfs4_proc_readdir(struct dentry *dentry, struct rpc_cred *cred,
		u64 cookie, struct page **pages, unsigned int count, int plus)
{
	struct inode		*dir = dentry->d_inode;
	struct nfs4_readdir_arg args = {
		.fh = NFS_FH(dir),
		.pages = pages,
		.pgbase = 0,
		.count = count,
		.bitmask = NFS_SERVER(dentry->d_inode)->attr_bitmask,
		.plus = plus,
	};
	struct nfs4_readdir_res res;
	struct rpc_message msg = {
		.rpc_proc = &nfs4_procedures[NFSPROC4_CLNT_READDIR],
		.rpc_argp = &args,
		.rpc_resp = &res,
		.rpc_cred = cred,
	};
	int			status;

	dprintk("%s: dentry = %s/%s, cookie = %Lu\n", __func__,
			dentry->d_parent->d_name.name,
			dentry->d_name.name,
			(unsigned long long)cookie);
	nfs4_setup_readdir(cookie, NFS_I(dir)->cookieverf, dentry, &args);
	res.pgbase = args.pgbase;
	status = nfs4_call_sync(NFS_SERVER(dir)->client, NFS_SERVER(dir), &msg, &args.seq_args, &res.seq_res, 0);
	if (status >= 0) {
		memcpy(NFS_I(dir)->cookieverf, res.verifier.data, NFS4_VERIFIER_SIZE);
		status += args.pgbase;
	}

	nfs_invalidate_atime(dir);

	dprintk("%s: returns %d\n", __func__, status);
	return status;
}
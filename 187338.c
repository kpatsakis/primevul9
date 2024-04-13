static int nfs4_proc_readdir(struct dentry *dentry, struct rpc_cred *cred,
		u64 cookie, struct page **pages, unsigned int count, int plus)
{
	struct nfs4_exception exception = { };
	int err;
	do {
		err = nfs4_handle_exception(NFS_SERVER(dentry->d_inode),
				_nfs4_proc_readdir(dentry, cred, cookie,
					pages, count, plus),
				&exception);
	} while (exception.retry);
	return err;
}
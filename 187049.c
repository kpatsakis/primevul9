int nfs4_proc_fs_locations(struct rpc_clnt *client, struct inode *dir,
			   const struct qstr *name,
			   struct nfs4_fs_locations *fs_locations,
			   struct page *page)
{
	struct nfs4_exception exception = { };
	int err;
	do {
		err = nfs4_handle_exception(NFS_SERVER(dir),
				_nfs4_proc_fs_locations(client, dir, name, fs_locations, page),
				&exception);
	} while (exception.retry);
	return err;
}
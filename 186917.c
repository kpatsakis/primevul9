nfs4_proc_lookup_mountpoint(struct inode *dir, struct qstr *name,
			    struct nfs_fh *fhandle, struct nfs_fattr *fattr)
{
	int status;
	struct rpc_clnt *client = rpc_clone_client(NFS_CLIENT(dir));

	status = nfs4_proc_lookup_common(&client, dir, name, fhandle, fattr);
	if (status < 0) {
		rpc_shutdown_client(client);
		return ERR_PTR(status);
	}
	return client;
}
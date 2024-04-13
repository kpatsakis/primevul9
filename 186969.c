static int nfs4_proc_lookup(struct inode *dir, struct qstr *name,
			    struct nfs_fh *fhandle, struct nfs_fattr *fattr)
{
	int status;
	struct rpc_clnt *client = NFS_CLIENT(dir);

	status = nfs4_proc_lookup_common(&client, dir, name, fhandle, fattr);
	if (client != NFS_CLIENT(dir)) {
		rpc_shutdown_client(client);
		nfs_fixup_secinfo_attributes(fattr);
	}
	return status;
}
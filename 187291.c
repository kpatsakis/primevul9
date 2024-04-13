static int nfs4_proc_lookup_common(struct rpc_clnt **clnt, struct inode *dir,
				   struct qstr *name, struct nfs_fh *fhandle,
				   struct nfs_fattr *fattr)
{
	struct nfs4_exception exception = { };
	struct rpc_clnt *client = *clnt;
	int err;
	do {
		err = _nfs4_proc_lookup(client, dir, name, fhandle, fattr);
		switch (err) {
		case -NFS4ERR_BADNAME:
			err = -ENOENT;
			goto out;
		case -NFS4ERR_MOVED:
			err = nfs4_get_referral(client, dir, name, fattr, fhandle);
			goto out;
		case -NFS4ERR_WRONGSEC:
			err = -EPERM;
			if (client != *clnt)
				goto out;

			client = nfs4_create_sec_client(client, dir, name);
			if (IS_ERR(client))
				return PTR_ERR(client);

			exception.retry = 1;
			break;
		default:
			err = nfs4_handle_exception(NFS_SERVER(dir), err, &exception);
		}
	} while (exception.retry);

out:
	if (err == 0)
		*clnt = client;
	else if (client != *clnt)
		rpc_shutdown_client(client);

	return err;
}
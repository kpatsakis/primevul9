static int _nfs4_proc_remove(struct inode *dir, struct qstr *name)
{
	struct nfs_server *server = NFS_SERVER(dir);
	struct nfs_removeargs args = {
		.fh = NFS_FH(dir),
		.name = *name,
	};
	struct nfs_removeres res = {
		.server = server,
	};
	struct rpc_message msg = {
		.rpc_proc = &nfs4_procedures[NFSPROC4_CLNT_REMOVE],
		.rpc_argp = &args,
		.rpc_resp = &res,
	};
	int status;

	status = nfs4_call_sync(server->client, server, &msg, &args.seq_args, &res.seq_res, 1);
	if (status == 0)
		update_changeattr(dir, &res.cinfo);
	return status;
}
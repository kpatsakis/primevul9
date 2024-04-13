static int _nfs4_proc_link(struct inode *inode, struct inode *dir, struct qstr *name)
{
	struct nfs_server *server = NFS_SERVER(inode);
	struct nfs4_link_arg arg = {
		.fh     = NFS_FH(inode),
		.dir_fh = NFS_FH(dir),
		.name   = name,
		.bitmask = server->attr_bitmask,
	};
	struct nfs4_link_res res = {
		.server = server,
	};
	struct rpc_message msg = {
		.rpc_proc = &nfs4_procedures[NFSPROC4_CLNT_LINK],
		.rpc_argp = &arg,
		.rpc_resp = &res,
	};
	int status = -ENOMEM;

	res.fattr = nfs_alloc_fattr();
	if (res.fattr == NULL)
		goto out;

	status = nfs4_call_sync(server->client, server, &msg, &arg.seq_args, &res.seq_res, 1);
	if (!status) {
		update_changeattr(dir, &res.cinfo);
		nfs_post_op_update_inode(inode, res.fattr);
	}
out:
	nfs_free_fattr(res.fattr);
	return status;
}
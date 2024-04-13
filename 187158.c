static int _nfs4_proc_lookup(struct rpc_clnt *clnt, struct inode *dir,
		const struct qstr *name, struct nfs_fh *fhandle,
		struct nfs_fattr *fattr)
{
	struct nfs_server *server = NFS_SERVER(dir);
	int		       status;
	struct nfs4_lookup_arg args = {
		.bitmask = server->attr_bitmask,
		.dir_fh = NFS_FH(dir),
		.name = name,
	};
	struct nfs4_lookup_res res = {
		.server = server,
		.fattr = fattr,
		.fh = fhandle,
	};
	struct rpc_message msg = {
		.rpc_proc = &nfs4_procedures[NFSPROC4_CLNT_LOOKUP],
		.rpc_argp = &args,
		.rpc_resp = &res,
	};

	nfs_fattr_init(fattr);

	dprintk("NFS call  lookup %s\n", name->name);
	status = nfs4_call_sync(clnt, server, &msg, &args.seq_args, &res.seq_res, 0);
	dprintk("NFS reply lookup: %d\n", status);
	return status;
}
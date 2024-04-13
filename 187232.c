static int _nfs4_proc_getattr(struct nfs_server *server, struct nfs_fh *fhandle, struct nfs_fattr *fattr)
{
	struct nfs4_getattr_arg args = {
		.fh = fhandle,
		.bitmask = server->attr_bitmask,
	};
	struct nfs4_getattr_res res = {
		.fattr = fattr,
		.server = server,
	};
	struct rpc_message msg = {
		.rpc_proc = &nfs4_procedures[NFSPROC4_CLNT_GETATTR],
		.rpc_argp = &args,
		.rpc_resp = &res,
	};
	
	nfs_fattr_init(fattr);
	return nfs4_call_sync(server->client, server, &msg, &args.seq_args, &res.seq_res, 0);
}
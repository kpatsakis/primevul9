static int _nfs4_getdevicelist(struct nfs_server *server,
				    const struct nfs_fh *fh,
				    struct pnfs_devicelist *devlist)
{
	struct nfs4_getdevicelist_args args = {
		.fh = fh,
		.layoutclass = server->pnfs_curr_ld->id,
	};
	struct nfs4_getdevicelist_res res = {
		.devlist = devlist,
	};
	struct rpc_message msg = {
		.rpc_proc = &nfs4_procedures[NFSPROC4_CLNT_GETDEVICELIST],
		.rpc_argp = &args,
		.rpc_resp = &res,
	};
	int status;

	dprintk("--> %s\n", __func__);
	status = nfs4_call_sync(server->client, server, &msg, &args.seq_args,
				&res.seq_res, 0);
	dprintk("<-- %s status=%d\n", __func__, status);
	return status;
}
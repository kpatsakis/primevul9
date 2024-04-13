_nfs4_proc_getdeviceinfo(struct nfs_server *server, struct pnfs_device *pdev)
{
	struct nfs4_getdeviceinfo_args args = {
		.pdev = pdev,
	};
	struct nfs4_getdeviceinfo_res res = {
		.pdev = pdev,
	};
	struct rpc_message msg = {
		.rpc_proc = &nfs4_procedures[NFSPROC4_CLNT_GETDEVICEINFO],
		.rpc_argp = &args,
		.rpc_resp = &res,
	};
	int status;

	dprintk("--> %s\n", __func__);
	status = nfs4_call_sync(server->client, server, &msg, &args.seq_args, &res.seq_res, 0);
	dprintk("<-- %s status=%d\n", __func__, status);

	return status;
}
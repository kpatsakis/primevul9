static int _nfs4_proc_destroy_clientid(struct nfs_client *clp,
		struct rpc_cred *cred)
{
	struct rpc_message msg = {
		.rpc_proc = &nfs4_procedures[NFSPROC4_CLNT_DESTROY_CLIENTID],
		.rpc_argp = clp,
		.rpc_cred = cred,
	};
	int status;

	status = rpc_call_sync(clp->cl_rpcclient, &msg, RPC_TASK_TIMEOUT);
	if (status)
		dprintk("NFS: Got error %d from the server %s on "
			"DESTROY_CLIENTID.", status, clp->cl_hostname);
	return status;
}
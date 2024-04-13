static int _nfs4_proc_create_session(struct nfs_client *clp,
		struct rpc_cred *cred)
{
	struct nfs4_session *session = clp->cl_session;
	struct nfs41_create_session_args args = {
		.client = clp,
		.cb_program = NFS4_CALLBACK,
	};
	struct nfs41_create_session_res res = {
		.client = clp,
	};
	struct rpc_message msg = {
		.rpc_proc = &nfs4_procedures[NFSPROC4_CLNT_CREATE_SESSION],
		.rpc_argp = &args,
		.rpc_resp = &res,
		.rpc_cred = cred,
	};
	int status;

	nfs4_init_channel_attrs(&args);
	args.flags = (SESSION4_PERSIST | SESSION4_BACK_CHAN);

	status = rpc_call_sync(session->clp->cl_rpcclient, &msg, RPC_TASK_TIMEOUT);

	if (!status)
		/* Verify the session's negotiated channel_attrs values */
		status = nfs4_verify_channel_attrs(&args, session);
	if (!status) {
		/* Increment the clientid slot sequence id */
		clp->cl_seqid++;
	}

	return status;
}
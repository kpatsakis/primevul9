static struct rpc_task *_nfs41_proc_sequence(struct nfs_client *clp, struct rpc_cred *cred)
{
	struct nfs4_sequence_data *calldata;
	struct rpc_message msg = {
		.rpc_proc = &nfs4_procedures[NFSPROC4_CLNT_SEQUENCE],
		.rpc_cred = cred,
	};
	struct rpc_task_setup task_setup_data = {
		.rpc_client = clp->cl_rpcclient,
		.rpc_message = &msg,
		.callback_ops = &nfs41_sequence_ops,
		.flags = RPC_TASK_ASYNC | RPC_TASK_SOFT,
	};

	if (!atomic_inc_not_zero(&clp->cl_count))
		return ERR_PTR(-EIO);
	calldata = kzalloc(sizeof(*calldata), GFP_NOFS);
	if (calldata == NULL) {
		nfs_put_client(clp);
		return ERR_PTR(-ENOMEM);
	}
	nfs41_init_sequence(&calldata->args, &calldata->res, 0);
	msg.rpc_argp = &calldata->args;
	msg.rpc_resp = &calldata->res;
	calldata->clp = clp;
	task_setup_data.callback_data = calldata;

	return rpc_run_task(&task_setup_data);
}
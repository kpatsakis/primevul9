int nfs4_proc_get_lease_time(struct nfs_client *clp, struct nfs_fsinfo *fsinfo)
{
	struct rpc_task *task;
	struct nfs4_get_lease_time_args args;
	struct nfs4_get_lease_time_res res = {
		.lr_fsinfo = fsinfo,
	};
	struct nfs4_get_lease_time_data data = {
		.args = &args,
		.res = &res,
		.clp = clp,
	};
	struct rpc_message msg = {
		.rpc_proc = &nfs4_procedures[NFSPROC4_CLNT_GET_LEASE_TIME],
		.rpc_argp = &args,
		.rpc_resp = &res,
	};
	struct rpc_task_setup task_setup = {
		.rpc_client = clp->cl_rpcclient,
		.rpc_message = &msg,
		.callback_ops = &nfs4_get_lease_time_ops,
		.callback_data = &data,
		.flags = RPC_TASK_TIMEOUT,
	};
	int status;

	nfs41_init_sequence(&args.la_seq_args, &res.lr_seq_res, 0);
	dprintk("--> %s\n", __func__);
	task = rpc_run_task(&task_setup);

	if (IS_ERR(task))
		status = PTR_ERR(task);
	else {
		status = task->tk_status;
		rpc_put_task(task);
	}
	dprintk("<-- %s return %d\n", __func__, status);

	return status;
}
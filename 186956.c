int nfs4_proc_layoutreturn(struct nfs4_layoutreturn *lrp)
{
	struct rpc_task *task;
	struct rpc_message msg = {
		.rpc_proc = &nfs4_procedures[NFSPROC4_CLNT_LAYOUTRETURN],
		.rpc_argp = &lrp->args,
		.rpc_resp = &lrp->res,
	};
	struct rpc_task_setup task_setup_data = {
		.rpc_client = lrp->clp->cl_rpcclient,
		.rpc_message = &msg,
		.callback_ops = &nfs4_layoutreturn_call_ops,
		.callback_data = lrp,
	};
	int status;

	dprintk("--> %s\n", __func__);
	nfs41_init_sequence(&lrp->args.seq_args, &lrp->res.seq_res, 1);
	task = rpc_run_task(&task_setup_data);
	if (IS_ERR(task))
		return PTR_ERR(task);
	status = task->tk_status;
	dprintk("<-- %s status=%d\n", __func__, status);
	rpc_put_task(task);
	return status;
}
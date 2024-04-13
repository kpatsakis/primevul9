static void nfs4_reclaim_complete_prepare(struct rpc_task *task, void *data)
{
	struct nfs4_reclaim_complete_data *calldata = data;

	rpc_task_set_priority(task, RPC_PRIORITY_PRIVILEGED);
	if (nfs41_setup_sequence(calldata->clp->cl_session,
				&calldata->arg.seq_args,
				&calldata->res.seq_res, task))
		return;

	rpc_call_start(task);
}
static int nfs4_call_sync_sequence(struct rpc_clnt *clnt,
				   struct nfs_server *server,
				   struct rpc_message *msg,
				   struct nfs4_sequence_args *args,
				   struct nfs4_sequence_res *res,
				   int privileged)
{
	int ret;
	struct rpc_task *task;
	struct nfs41_call_sync_data data = {
		.seq_server = server,
		.seq_args = args,
		.seq_res = res,
	};
	struct rpc_task_setup task_setup = {
		.rpc_client = clnt,
		.rpc_message = msg,
		.callback_ops = &nfs41_call_sync_ops,
		.callback_data = &data
	};

	if (privileged)
		task_setup.callback_ops = &nfs41_call_priv_sync_ops;
	task = rpc_run_task(&task_setup);
	if (IS_ERR(task))
		ret = PTR_ERR(task);
	else {
		ret = task->tk_status;
		rpc_put_task(task);
	}
	return ret;
}
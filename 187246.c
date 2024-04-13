nfs4_proc_layoutcommit(struct nfs4_layoutcommit_data *data, bool sync)
{
	struct rpc_message msg = {
		.rpc_proc = &nfs4_procedures[NFSPROC4_CLNT_LAYOUTCOMMIT],
		.rpc_argp = &data->args,
		.rpc_resp = &data->res,
		.rpc_cred = data->cred,
	};
	struct rpc_task_setup task_setup_data = {
		.task = &data->task,
		.rpc_client = NFS_CLIENT(data->args.inode),
		.rpc_message = &msg,
		.callback_ops = &nfs4_layoutcommit_ops,
		.callback_data = data,
		.flags = RPC_TASK_ASYNC,
	};
	struct rpc_task *task;
	int status = 0;

	dprintk("NFS: %4d initiating layoutcommit call. sync %d "
		"lbw: %llu inode %lu\n",
		data->task.tk_pid, sync,
		data->args.lastbytewritten,
		data->args.inode->i_ino);

	nfs41_init_sequence(&data->args.seq_args, &data->res.seq_res, 1);
	task = rpc_run_task(&task_setup_data);
	if (IS_ERR(task))
		return PTR_ERR(task);
	if (sync == false)
		goto out;
	status = nfs4_wait_for_completion_rpc_task(task);
	if (status != 0)
		goto out;
	status = task->tk_status;
out:
	dprintk("%s: status %d\n", __func__, status);
	rpc_put_task(task);
	return status;
}
static int _nfs4_do_setlk(struct nfs4_state *state, int cmd, struct file_lock *fl, int recovery_type)
{
	struct nfs4_lockdata *data;
	struct rpc_task *task;
	struct rpc_message msg = {
		.rpc_proc = &nfs4_procedures[NFSPROC4_CLNT_LOCK],
		.rpc_cred = state->owner->so_cred,
	};
	struct rpc_task_setup task_setup_data = {
		.rpc_client = NFS_CLIENT(state->inode),
		.rpc_message = &msg,
		.callback_ops = &nfs4_lock_ops,
		.workqueue = nfsiod_workqueue,
		.flags = RPC_TASK_ASYNC,
	};
	int ret;

	dprintk("%s: begin!\n", __func__);
	data = nfs4_alloc_lockdata(fl, nfs_file_open_context(fl->fl_file),
			fl->fl_u.nfs4_fl.owner,
			recovery_type == NFS_LOCK_NEW ? GFP_KERNEL : GFP_NOFS);
	if (data == NULL)
		return -ENOMEM;
	if (IS_SETLKW(cmd))
		data->arg.block = 1;
	if (recovery_type > NFS_LOCK_NEW) {
		if (recovery_type == NFS_LOCK_RECLAIM)
			data->arg.reclaim = NFS_LOCK_RECLAIM;
		task_setup_data.callback_ops = &nfs4_recover_lock_ops;
	}
	nfs41_init_sequence(&data->arg.seq_args, &data->res.seq_res, 1);
	msg.rpc_argp = &data->arg;
	msg.rpc_resp = &data->res;
	task_setup_data.callback_data = data;
	task = rpc_run_task(&task_setup_data);
	if (IS_ERR(task))
		return PTR_ERR(task);
	ret = nfs4_wait_for_completion_rpc_task(task);
	if (ret == 0) {
		ret = data->rpc_status;
		if (ret)
			nfs4_handle_setlk_error(data->server, data->lsp,
					data->arg.new_lock_owner, ret);
	} else
		data->cancelled = 1;
	rpc_put_task(task);
	dprintk("%s: done, ret = %d!\n", __func__, ret);
	return ret;
}
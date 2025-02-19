static int nfs4_proc_sequence(struct nfs_client *clp, struct rpc_cred *cred)
{
	struct rpc_task *task;
	int ret;

	task = _nfs41_proc_sequence(clp, cred);
	if (IS_ERR(task)) {
		ret = PTR_ERR(task);
		goto out;
	}
	ret = rpc_wait_for_completion_task(task);
	if (!ret) {
		struct nfs4_sequence_res *res = task->tk_msg.rpc_resp;

		if (task->tk_status == 0)
			nfs41_handle_sequence_flag_errors(clp, res->sr_status_flags);
		ret = task->tk_status;
	}
	rpc_put_task(task);
out:
	dprintk("<-- %s status=%d\n", __func__, ret);
	return ret;
}
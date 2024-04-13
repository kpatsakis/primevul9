static void nfs41_sequence_call_done(struct rpc_task *task, void *data)
{
	struct nfs4_sequence_data *calldata = data;
	struct nfs_client *clp = calldata->clp;

	if (!nfs41_sequence_done(task, task->tk_msg.rpc_resp))
		return;

	if (task->tk_status < 0) {
		dprintk("%s ERROR %d\n", __func__, task->tk_status);
		if (atomic_read(&clp->cl_count) == 1)
			goto out;

		if (nfs41_sequence_handle_errors(task, clp) == -EAGAIN) {
			rpc_restart_call_prepare(task);
			return;
		}
	}
	dprintk("%s rpc_cred %p\n", __func__, task->tk_msg.rpc_cred);
out:
	dprintk("<-- %s\n", __func__);
}
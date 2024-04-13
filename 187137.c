static void nfs4_reclaim_complete_done(struct rpc_task *task, void *data)
{
	struct nfs4_reclaim_complete_data *calldata = data;
	struct nfs_client *clp = calldata->clp;
	struct nfs4_sequence_res *res = &calldata->res.seq_res;

	dprintk("--> %s\n", __func__);
	if (!nfs41_sequence_done(task, res))
		return;

	if (nfs41_reclaim_complete_handle_errors(task, clp) == -EAGAIN) {
		rpc_restart_call_prepare(task);
		return;
	}
	dprintk("<-- %s\n", __func__);
}
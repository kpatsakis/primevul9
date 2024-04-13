static void nfs41_call_sync_prepare(struct rpc_task *task, void *calldata)
{
	struct nfs41_call_sync_data *data = calldata;

	dprintk("--> %s data->seq_server %p\n", __func__, data->seq_server);

	if (nfs4_setup_sequence(data->seq_server, data->seq_args,
				data->seq_res, task))
		return;
	rpc_call_start(task);
}
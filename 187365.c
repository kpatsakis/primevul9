static void nfs41_call_sync_done(struct rpc_task *task, void *calldata)
{
	struct nfs41_call_sync_data *data = calldata;

	nfs41_sequence_done(task, data->seq_res);
}
static void nfs41_call_priv_sync_prepare(struct rpc_task *task, void *calldata)
{
	rpc_task_set_priority(task, RPC_PRIORITY_PRIVILEGED);
	nfs41_call_sync_prepare(task, calldata);
}
static void nfs4_recover_open_prepare(struct rpc_task *task, void *calldata)
{
	rpc_task_set_priority(task, RPC_PRIORITY_PRIVILEGED);
	nfs4_open_prepare(task, calldata);
}
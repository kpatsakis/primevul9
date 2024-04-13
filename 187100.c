bool nfs4_set_task_privileged(struct rpc_task *task, void *dummy)
{
	rpc_task_set_priority(task, RPC_PRIORITY_PRIVILEGED);
	return true;
}
static int nfs4_wait_for_completion_rpc_task(struct rpc_task *task)
{
	int ret;

	ret = rpc_wait_for_completion_task(task);
	return ret;
}
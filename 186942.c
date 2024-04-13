static void nfs4_get_lease_time_prepare(struct rpc_task *task,
					void *calldata)
{
	int ret;
	struct nfs4_get_lease_time_data *data =
			(struct nfs4_get_lease_time_data *)calldata;

	dprintk("--> %s\n", __func__);
	rpc_task_set_priority(task, RPC_PRIORITY_PRIVILEGED);
	/* just setup sequence, do not trigger session recovery
	   since we're invoked within one */
	ret = nfs41_setup_sequence(data->clp->cl_session,
				   &data->args->la_seq_args,
				   &data->res->lr_seq_res, task);

	BUG_ON(ret == -EAGAIN);
	rpc_call_start(task);
	dprintk("<-- %s\n", __func__);
}
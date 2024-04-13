static void nfs4_delegreturn_done(struct rpc_task *task, void *calldata)
{
	struct nfs4_delegreturndata *data = calldata;

	if (!nfs4_sequence_done(task, &data->res.seq_res))
		return;

	switch (task->tk_status) {
	case -NFS4ERR_STALE_STATEID:
	case -NFS4ERR_EXPIRED:
	case 0:
		renew_lease(data->res.server, data->timestamp);
		break;
	default:
		if (nfs4_async_handle_error(task, data->res.server, NULL) ==
				-EAGAIN) {
			rpc_restart_call_prepare(task);
			return;
		}
	}
	data->rpc_status = task->tk_status;
}
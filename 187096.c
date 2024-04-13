static void nfs4_locku_done(struct rpc_task *task, void *data)
{
	struct nfs4_unlockdata *calldata = data;

	if (!nfs4_sequence_done(task, &calldata->res.seq_res))
		return;
	switch (task->tk_status) {
		case 0:
			nfs4_stateid_copy(&calldata->lsp->ls_stateid,
					&calldata->res.stateid);
			renew_lease(calldata->server, calldata->timestamp);
			break;
		case -NFS4ERR_BAD_STATEID:
		case -NFS4ERR_OLD_STATEID:
		case -NFS4ERR_STALE_STATEID:
		case -NFS4ERR_EXPIRED:
			break;
		default:
			if (nfs4_async_handle_error(task, calldata->server, NULL) == -EAGAIN)
				rpc_restart_call_prepare(task);
	}
}
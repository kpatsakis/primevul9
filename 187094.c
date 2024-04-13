nfs4_async_handle_error(struct rpc_task *task, const struct nfs_server *server, struct nfs4_state *state)
{
	struct nfs_client *clp = server->nfs_client;

	if (task->tk_status >= 0)
		return 0;
	switch(task->tk_status) {
		case -NFS4ERR_DELEG_REVOKED:
		case -NFS4ERR_ADMIN_REVOKED:
		case -NFS4ERR_BAD_STATEID:
			if (state == NULL)
				break;
			nfs_remove_bad_delegation(state->inode);
		case -NFS4ERR_OPENMODE:
			if (state == NULL)
				break;
			nfs4_schedule_stateid_recovery(server, state);
			goto wait_on_recovery;
		case -NFS4ERR_EXPIRED:
			if (state != NULL)
				nfs4_schedule_stateid_recovery(server, state);
		case -NFS4ERR_STALE_STATEID:
		case -NFS4ERR_STALE_CLIENTID:
			nfs4_schedule_lease_recovery(clp);
			goto wait_on_recovery;
#if defined(CONFIG_NFS_V4_1)
		case -NFS4ERR_BADSESSION:
		case -NFS4ERR_BADSLOT:
		case -NFS4ERR_BAD_HIGH_SLOT:
		case -NFS4ERR_DEADSESSION:
		case -NFS4ERR_CONN_NOT_BOUND_TO_SESSION:
		case -NFS4ERR_SEQ_FALSE_RETRY:
		case -NFS4ERR_SEQ_MISORDERED:
			dprintk("%s ERROR %d, Reset session\n", __func__,
				task->tk_status);
			nfs4_schedule_session_recovery(clp->cl_session, task->tk_status);
			task->tk_status = 0;
			return -EAGAIN;
#endif /* CONFIG_NFS_V4_1 */
		case -NFS4ERR_DELAY:
			nfs_inc_server_stats(server, NFSIOS_DELAY);
		case -NFS4ERR_GRACE:
		case -EKEYEXPIRED:
			rpc_delay(task, NFS4_POLL_RETRY_MAX);
			task->tk_status = 0;
			return -EAGAIN;
		case -NFS4ERR_RETRY_UNCACHED_REP:
		case -NFS4ERR_OLD_STATEID:
			task->tk_status = 0;
			return -EAGAIN;
	}
	task->tk_status = nfs4_map_errors(task->tk_status);
	return 0;
wait_on_recovery:
	rpc_sleep_on(&clp->cl_rpcwaitq, task, NULL);
	if (test_bit(NFS4CLNT_MANAGER_RUNNING, &clp->cl_state) == 0)
		rpc_wake_up_queued_task(&clp->cl_rpcwaitq, task);
	task->tk_status = 0;
	return -EAGAIN;
}
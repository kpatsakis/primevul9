static int nfs41_reclaim_complete_handle_errors(struct rpc_task *task, struct nfs_client *clp)
{
	switch(task->tk_status) {
	case 0:
	case -NFS4ERR_COMPLETE_ALREADY:
	case -NFS4ERR_WRONG_CRED: /* What to do here? */
		break;
	case -NFS4ERR_DELAY:
		rpc_delay(task, NFS4_POLL_RETRY_MAX);
		/* fall through */
	case -NFS4ERR_RETRY_UNCACHED_REP:
		return -EAGAIN;
	default:
		nfs4_schedule_lease_recovery(clp);
	}
	return 0;
}
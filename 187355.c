static int nfs41_sequence_done(struct rpc_task *task, struct nfs4_sequence_res *res)
{
	unsigned long timestamp;
	struct nfs_client *clp;

	/*
	 * sr_status remains 1 if an RPC level error occurred. The server
	 * may or may not have processed the sequence operation..
	 * Proceed as if the server received and processed the sequence
	 * operation.
	 */
	if (res->sr_status == 1)
		res->sr_status = NFS_OK;

	/* don't increment the sequence number if the task wasn't sent */
	if (!RPC_WAS_SENT(task))
		goto out;

	/* Check the SEQUENCE operation status */
	switch (res->sr_status) {
	case 0:
		/* Update the slot's sequence and clientid lease timer */
		++res->sr_slot->seq_nr;
		timestamp = res->sr_renewal_time;
		clp = res->sr_session->clp;
		do_renew_lease(clp, timestamp);
		/* Check sequence flags */
		if (res->sr_status_flags != 0)
			nfs4_schedule_lease_recovery(clp);
		break;
	case -NFS4ERR_DELAY:
		/* The server detected a resend of the RPC call and
		 * returned NFS4ERR_DELAY as per Section 2.10.6.2
		 * of RFC5661.
		 */
		dprintk("%s: slot=%td seq=%d: Operation in progress\n",
			__func__,
			res->sr_slot - res->sr_session->fc_slot_table.slots,
			res->sr_slot->seq_nr);
		goto out_retry;
	default:
		/* Just update the slot sequence no. */
		++res->sr_slot->seq_nr;
	}
out:
	/* The session may be reset by one of the error handlers. */
	dprintk("%s: Error %d free the slot \n", __func__, res->sr_status);
	nfs41_sequence_free_slot(res);
	return 1;
out_retry:
	if (!rpc_restart_call(task))
		goto out;
	rpc_delay(task, NFS4_POLL_RETRY_MAX);
	return 0;
}
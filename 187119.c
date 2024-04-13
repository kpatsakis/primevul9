static void nfs4_close_prepare(struct rpc_task *task, void *data)
{
	struct nfs4_closedata *calldata = data;
	struct nfs4_state *state = calldata->state;
	int call_close = 0;

	dprintk("%s: begin!\n", __func__);
	if (nfs_wait_on_sequence(calldata->arg.seqid, task) != 0)
		return;

	task->tk_msg.rpc_proc = &nfs4_procedures[NFSPROC4_CLNT_OPEN_DOWNGRADE];
	calldata->arg.fmode = FMODE_READ|FMODE_WRITE;
	spin_lock(&state->owner->so_lock);
	/* Calculate the change in open mode */
	if (state->n_rdwr == 0) {
		if (state->n_rdonly == 0) {
			call_close |= test_bit(NFS_O_RDONLY_STATE, &state->flags);
			call_close |= test_bit(NFS_O_RDWR_STATE, &state->flags);
			calldata->arg.fmode &= ~FMODE_READ;
		}
		if (state->n_wronly == 0) {
			call_close |= test_bit(NFS_O_WRONLY_STATE, &state->flags);
			call_close |= test_bit(NFS_O_RDWR_STATE, &state->flags);
			calldata->arg.fmode &= ~FMODE_WRITE;
		}
	}
	spin_unlock(&state->owner->so_lock);

	if (!call_close) {
		/* Note: exit _without_ calling nfs4_close_done */
		task->tk_action = NULL;
		goto out;
	}

	if (calldata->arg.fmode == 0) {
		task->tk_msg.rpc_proc = &nfs4_procedures[NFSPROC4_CLNT_CLOSE];
		if (calldata->roc &&
		    pnfs_roc_drain(calldata->inode, &calldata->roc_barrier)) {
			rpc_sleep_on(&NFS_SERVER(calldata->inode)->roc_rpcwaitq,
				     task, NULL);
			goto out;
		}
	}

	nfs_fattr_init(calldata->res.fattr);
	calldata->timestamp = jiffies;
	if (nfs4_setup_sequence(NFS_SERVER(calldata->inode),
				&calldata->arg.seq_args,
				&calldata->res.seq_res,
				task))
		goto out;
	rpc_call_start(task);
out:
	dprintk("%s: done!\n", __func__);
}
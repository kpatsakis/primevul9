static void nfs4_close_done(struct rpc_task *task, void *data)
{
	struct nfs4_closedata *calldata = data;
	struct nfs4_state *state = calldata->state;
	struct nfs_server *server = NFS_SERVER(calldata->inode);

	dprintk("%s: begin!\n", __func__);
	if (!nfs4_sequence_done(task, &calldata->res.seq_res))
		return;
        /* hmm. we are done with the inode, and in the process of freeing
	 * the state_owner. we keep this around to process errors
	 */
	switch (task->tk_status) {
		case 0:
			if (calldata->roc)
				pnfs_roc_set_barrier(state->inode,
						     calldata->roc_barrier);
			nfs_set_open_stateid(state, &calldata->res.stateid, 0);
			renew_lease(server, calldata->timestamp);
			nfs4_close_clear_stateid_flags(state,
					calldata->arg.fmode);
			break;
		case -NFS4ERR_STALE_STATEID:
		case -NFS4ERR_OLD_STATEID:
		case -NFS4ERR_BAD_STATEID:
		case -NFS4ERR_EXPIRED:
			if (calldata->arg.fmode == 0)
				break;
		default:
			if (nfs4_async_handle_error(task, server, state) == -EAGAIN)
				rpc_restart_call_prepare(task);
	}
	nfs_release_seqid(calldata->arg.seqid);
	nfs_refresh_inode(calldata->inode, calldata->res.fattr);
	dprintk("%s: done, ret = %d!\n", __func__, task->tk_status);
}
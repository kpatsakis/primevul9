int nfs4_do_close(struct nfs4_state *state, gfp_t gfp_mask, int wait, bool roc)
{
	struct nfs_server *server = NFS_SERVER(state->inode);
	struct nfs4_closedata *calldata;
	struct nfs4_state_owner *sp = state->owner;
	struct rpc_task *task;
	struct rpc_message msg = {
		.rpc_proc = &nfs4_procedures[NFSPROC4_CLNT_CLOSE],
		.rpc_cred = state->owner->so_cred,
	};
	struct rpc_task_setup task_setup_data = {
		.rpc_client = server->client,
		.rpc_message = &msg,
		.callback_ops = &nfs4_close_ops,
		.workqueue = nfsiod_workqueue,
		.flags = RPC_TASK_ASYNC,
	};
	int status = -ENOMEM;

	calldata = kzalloc(sizeof(*calldata), gfp_mask);
	if (calldata == NULL)
		goto out;
	nfs41_init_sequence(&calldata->arg.seq_args, &calldata->res.seq_res, 1);
	calldata->inode = state->inode;
	calldata->state = state;
	calldata->arg.fh = NFS_FH(state->inode);
	calldata->arg.stateid = &state->open_stateid;
	/* Serialization for the sequence id */
	calldata->arg.seqid = nfs_alloc_seqid(&state->owner->so_seqid, gfp_mask);
	if (calldata->arg.seqid == NULL)
		goto out_free_calldata;
	calldata->arg.fmode = 0;
	calldata->arg.bitmask = server->cache_consistency_bitmask;
	calldata->res.fattr = &calldata->fattr;
	calldata->res.seqid = calldata->arg.seqid;
	calldata->res.server = server;
	calldata->roc = roc;
	nfs_sb_active(calldata->inode->i_sb);

	msg.rpc_argp = &calldata->arg;
	msg.rpc_resp = &calldata->res;
	task_setup_data.callback_data = calldata;
	task = rpc_run_task(&task_setup_data);
	if (IS_ERR(task))
		return PTR_ERR(task);
	status = 0;
	if (wait)
		status = rpc_wait_for_completion_task(task);
	rpc_put_task(task);
	return status;
out_free_calldata:
	kfree(calldata);
out:
	if (roc)
		pnfs_roc_release(state->inode);
	nfs4_put_open_state(state);
	nfs4_put_state_owner(sp);
	return status;
}
static struct rpc_task *nfs4_do_unlck(struct file_lock *fl,
		struct nfs_open_context *ctx,
		struct nfs4_lock_state *lsp,
		struct nfs_seqid *seqid)
{
	struct nfs4_unlockdata *data;
	struct rpc_message msg = {
		.rpc_proc = &nfs4_procedures[NFSPROC4_CLNT_LOCKU],
		.rpc_cred = ctx->cred,
	};
	struct rpc_task_setup task_setup_data = {
		.rpc_client = NFS_CLIENT(lsp->ls_state->inode),
		.rpc_message = &msg,
		.callback_ops = &nfs4_locku_ops,
		.workqueue = nfsiod_workqueue,
		.flags = RPC_TASK_ASYNC,
	};

	/* Ensure this is an unlock - when canceling a lock, the
	 * canceled lock is passed in, and it won't be an unlock.
	 */
	fl->fl_type = F_UNLCK;

	data = nfs4_alloc_unlockdata(fl, ctx, lsp, seqid);
	if (data == NULL) {
		nfs_free_seqid(seqid);
		return ERR_PTR(-ENOMEM);
	}

	nfs41_init_sequence(&data->arg.seq_args, &data->res.seq_res, 1);
	msg.rpc_argp = &data->arg;
	msg.rpc_resp = &data->res;
	task_setup_data.callback_data = data;
	return rpc_run_task(&task_setup_data);
}
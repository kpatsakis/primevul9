void nfs4_proc_layoutget(struct nfs4_layoutget *lgp, gfp_t gfp_flags)
{
	struct nfs_server *server = NFS_SERVER(lgp->args.inode);
	size_t max_pages = max_response_pages(server);
	struct rpc_task *task;
	struct rpc_message msg = {
		.rpc_proc = &nfs4_procedures[NFSPROC4_CLNT_LAYOUTGET],
		.rpc_argp = &lgp->args,
		.rpc_resp = &lgp->res,
	};
	struct rpc_task_setup task_setup_data = {
		.rpc_client = server->client,
		.rpc_message = &msg,
		.callback_ops = &nfs4_layoutget_call_ops,
		.callback_data = lgp,
		.flags = RPC_TASK_ASYNC,
	};
	int status = 0;

	dprintk("--> %s\n", __func__);

	lgp->args.layout.pages = nfs4_alloc_pages(max_pages, gfp_flags);
	if (!lgp->args.layout.pages) {
		nfs4_layoutget_release(lgp);
		return;
	}
	lgp->args.layout.pglen = max_pages * PAGE_SIZE;

	lgp->res.layoutp = &lgp->args.layout;
	lgp->res.seq_res.sr_slot = NULL;
	nfs41_init_sequence(&lgp->args.seq_args, &lgp->res.seq_res, 0);
	task = rpc_run_task(&task_setup_data);
	if (IS_ERR(task))
		return;
	status = nfs4_wait_for_completion_rpc_task(task);
	if (status == 0)
		status = task->tk_status;
	if (status == 0)
		status = pnfs_layout_process(lgp);
	rpc_put_task(task);
	dprintk("<-- %s status=%d\n", __func__, status);
	return;
}
nfs4_layoutget_prepare(struct rpc_task *task, void *calldata)
{
	struct nfs4_layoutget *lgp = calldata;
	struct nfs_server *server = NFS_SERVER(lgp->args.inode);

	dprintk("--> %s\n", __func__);
	/* Note the is a race here, where a CB_LAYOUTRECALL can come in
	 * right now covering the LAYOUTGET we are about to send.
	 * However, that is not so catastrophic, and there seems
	 * to be no way to prevent it completely.
	 */
	if (nfs4_setup_sequence(server, &lgp->args.seq_args,
				&lgp->res.seq_res, task))
		return;
	if (pnfs_choose_layoutget_stateid(&lgp->args.stateid,
					  NFS_I(lgp->args.inode)->layout,
					  lgp->args.ctx->state)) {
		rpc_exit(task, NFS4_OK);
		return;
	}
	rpc_call_start(task);
}
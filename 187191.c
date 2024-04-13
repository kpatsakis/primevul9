static void nfs4_layoutreturn_done(struct rpc_task *task, void *calldata)
{
	struct nfs4_layoutreturn *lrp = calldata;
	struct nfs_server *server;
	struct pnfs_layout_hdr *lo = lrp->args.layout;

	dprintk("--> %s\n", __func__);

	if (!nfs4_sequence_done(task, &lrp->res.seq_res))
		return;

	server = NFS_SERVER(lrp->args.inode);
	if (nfs4_async_handle_error(task, server, NULL) == -EAGAIN) {
		rpc_restart_call_prepare(task);
		return;
	}
	spin_lock(&lo->plh_inode->i_lock);
	if (task->tk_status == 0 && lrp->res.lrs_present)
		pnfs_set_layout_stateid(lo, &lrp->res.stateid, true);
	lo->plh_block_lgets--;
	spin_unlock(&lo->plh_inode->i_lock);
	dprintk("<-- %s\n", __func__);
}
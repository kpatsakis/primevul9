static void nfs4_layoutget_done(struct rpc_task *task, void *calldata)
{
	struct nfs4_layoutget *lgp = calldata;
	struct nfs_server *server = NFS_SERVER(lgp->args.inode);

	dprintk("--> %s\n", __func__);

	if (!nfs4_sequence_done(task, &lgp->res.seq_res))
		return;

	switch (task->tk_status) {
	case 0:
		break;
	case -NFS4ERR_LAYOUTTRYLATER:
	case -NFS4ERR_RECALLCONFLICT:
		task->tk_status = -NFS4ERR_DELAY;
		/* Fall through */
	default:
		if (nfs4_async_handle_error(task, server, NULL) == -EAGAIN) {
			rpc_restart_call_prepare(task);
			return;
		}
	}
	dprintk("<-- %s\n", __func__);
}
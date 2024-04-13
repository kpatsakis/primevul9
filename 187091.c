nfs4_layoutcommit_done(struct rpc_task *task, void *calldata)
{
	struct nfs4_layoutcommit_data *data = calldata;
	struct nfs_server *server = NFS_SERVER(data->args.inode);

	if (!nfs4_sequence_done(task, &data->res.seq_res))
		return;

	switch (task->tk_status) { /* Just ignore these failures */
	case -NFS4ERR_DELEG_REVOKED: /* layout was recalled */
	case -NFS4ERR_BADIOMODE:     /* no IOMODE_RW layout for range */
	case -NFS4ERR_BADLAYOUT:     /* no layout */
	case -NFS4ERR_GRACE:	    /* loca_recalim always false */
		task->tk_status = 0;
		break;
	case 0:
		nfs_post_op_update_inode_force_wcc(data->args.inode,
						   data->res.fattr);
		break;
	default:
		if (nfs4_async_handle_error(task, server, NULL) == -EAGAIN) {
			rpc_restart_call_prepare(task);
			return;
		}
	}
}
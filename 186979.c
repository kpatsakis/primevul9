static int nfs4_write_done_cb(struct rpc_task *task, struct nfs_write_data *data)
{
	struct inode *inode = data->header->inode;
	
	if (nfs4_async_handle_error(task, NFS_SERVER(inode), data->args.context->state) == -EAGAIN) {
		rpc_restart_call_prepare(task);
		return -EAGAIN;
	}
	if (task->tk_status >= 0) {
		renew_lease(NFS_SERVER(inode), data->timestamp);
		nfs_post_op_update_inode_force_wcc(inode, &data->fattr);
	}
	return 0;
}
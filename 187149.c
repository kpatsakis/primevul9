static int nfs4_commit_done_cb(struct rpc_task *task, struct nfs_commit_data *data)
{
	struct inode *inode = data->inode;

	if (nfs4_async_handle_error(task, NFS_SERVER(inode), NULL) == -EAGAIN) {
		rpc_restart_call_prepare(task);
		return -EAGAIN;
	}
	return 0;
}
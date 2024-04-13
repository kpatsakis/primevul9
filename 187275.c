static int nfs4_proc_unlink_done(struct rpc_task *task, struct inode *dir)
{
	struct nfs_removeres *res = task->tk_msg.rpc_resp;

	if (!nfs4_sequence_done(task, &res->seq_res))
		return 0;
	if (nfs4_async_handle_error(task, res->server, NULL) == -EAGAIN)
		return 0;
	update_changeattr(dir, &res->cinfo);
	return 1;
}
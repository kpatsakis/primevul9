static int nfs4_proc_unlck(struct nfs4_state *state, int cmd, struct file_lock *request)
{
	struct nfs_inode *nfsi = NFS_I(state->inode);
	struct nfs_seqid *seqid;
	struct nfs4_lock_state *lsp;
	struct rpc_task *task;
	int status = 0;
	unsigned char fl_flags = request->fl_flags;

	status = nfs4_set_lock_state(state, request);
	/* Unlock _before_ we do the RPC call */
	request->fl_flags |= FL_EXISTS;
	down_read(&nfsi->rwsem);
	if (do_vfs_lock(request->fl_file, request) == -ENOENT) {
		up_read(&nfsi->rwsem);
		goto out;
	}
	up_read(&nfsi->rwsem);
	if (status != 0)
		goto out;
	/* Is this a delegated lock? */
	if (test_bit(NFS_DELEGATED_STATE, &state->flags))
		goto out;
	lsp = request->fl_u.nfs4_fl.owner;
	seqid = nfs_alloc_seqid(&lsp->ls_seqid, GFP_KERNEL);
	status = -ENOMEM;
	if (seqid == NULL)
		goto out;
	task = nfs4_do_unlck(request, nfs_file_open_context(request->fl_file), lsp, seqid);
	status = PTR_ERR(task);
	if (IS_ERR(task))
		goto out;
	status = nfs4_wait_for_completion_rpc_task(task);
	rpc_put_task(task);
out:
	request->fl_flags = fl_flags;
	return status;
}
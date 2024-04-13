static int _nfs4_proc_setlk(struct nfs4_state *state, int cmd, struct file_lock *request)
{
	struct nfs_inode *nfsi = NFS_I(state->inode);
	unsigned char fl_flags = request->fl_flags;
	int status = -ENOLCK;

	if ((fl_flags & FL_POSIX) &&
			!test_bit(NFS_STATE_POSIX_LOCKS, &state->flags))
		goto out;
	/* Is this a delegated open? */
	status = nfs4_set_lock_state(state, request);
	if (status != 0)
		goto out;
	request->fl_flags |= FL_ACCESS;
	status = do_vfs_lock(request->fl_file, request);
	if (status < 0)
		goto out;
	down_read(&nfsi->rwsem);
	if (test_bit(NFS_DELEGATED_STATE, &state->flags)) {
		/* Yes: cache locks! */
		/* ...but avoid races with delegation recall... */
		request->fl_flags = fl_flags & ~FL_SLEEP;
		status = do_vfs_lock(request->fl_file, request);
		goto out_unlock;
	}
	status = _nfs4_do_setlk(state, cmd, request, NFS_LOCK_NEW);
	if (status != 0)
		goto out_unlock;
	/* Note: we always want to sleep here! */
	request->fl_flags = fl_flags | FL_SLEEP;
	if (do_vfs_lock(request->fl_file, request) < 0)
		printk(KERN_WARNING "NFS: %s: VFS is out of sync with lock "
			"manager!\n", __func__);
out_unlock:
	up_read(&nfsi->rwsem);
out:
	request->fl_flags = fl_flags;
	return status;
}
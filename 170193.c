xfs_iolock_two_inodes_and_break_layout(
	struct inode		*src,
	struct inode		*dest)
{
	int			error;

	if (src > dest)
		swap(src, dest);

retry:
	/* Wait to break both inodes' layouts before we start locking. */
	error = break_layout(src, true);
	if (error)
		return error;
	if (src != dest) {
		error = break_layout(dest, true);
		if (error)
			return error;
	}

	/* Lock one inode and make sure nobody got in and leased it. */
	inode_lock(src);
	error = break_layout(src, false);
	if (error) {
		inode_unlock(src);
		if (error == -EWOULDBLOCK)
			goto retry;
		return error;
	}

	if (src == dest)
		return 0;

	/* Lock the other inode and make sure nobody got in and leased it. */
	inode_lock_nested(dest, I_MUTEX_NONDIR2);
	error = break_layout(dest, false);
	if (error) {
		inode_unlock(src);
		inode_unlock(dest);
		if (error == -EWOULDBLOCK)
			goto retry;
		return error;
	}

	return 0;
}
xfs_release(
	xfs_inode_t	*ip)
{
	xfs_mount_t	*mp = ip->i_mount;
	int		error;

	if (!S_ISREG(VFS_I(ip)->i_mode) || (VFS_I(ip)->i_mode == 0))
		return 0;

	/* If this is a read-only mount, don't do this (would generate I/O) */
	if (mp->m_flags & XFS_MOUNT_RDONLY)
		return 0;

	if (!XFS_FORCED_SHUTDOWN(mp)) {
		int truncated;

		/*
		 * If we previously truncated this file and removed old data
		 * in the process, we want to initiate "early" writeout on
		 * the last close.  This is an attempt to combat the notorious
		 * NULL files problem which is particularly noticeable from a
		 * truncate down, buffered (re-)write (delalloc), followed by
		 * a crash.  What we are effectively doing here is
		 * significantly reducing the time window where we'd otherwise
		 * be exposed to that problem.
		 */
		truncated = xfs_iflags_test_and_clear(ip, XFS_ITRUNCATED);
		if (truncated) {
			xfs_iflags_clear(ip, XFS_IDIRTY_RELEASE);
			if (ip->i_delayed_blks > 0) {
				error = filemap_flush(VFS_I(ip)->i_mapping);
				if (error)
					return error;
			}
		}
	}

	if (VFS_I(ip)->i_nlink == 0)
		return 0;

	if (xfs_can_free_eofblocks(ip, false)) {

		/*
		 * Check if the inode is being opened, written and closed
		 * frequently and we have delayed allocation blocks outstanding
		 * (e.g. streaming writes from the NFS server), truncating the
		 * blocks past EOF will cause fragmentation to occur.
		 *
		 * In this case don't do the truncation, but we have to be
		 * careful how we detect this case. Blocks beyond EOF show up as
		 * i_delayed_blks even when the inode is clean, so we need to
		 * truncate them away first before checking for a dirty release.
		 * Hence on the first dirty close we will still remove the
		 * speculative allocation, but after that we will leave it in
		 * place.
		 */
		if (xfs_iflags_test(ip, XFS_IDIRTY_RELEASE))
			return 0;
		/*
		 * If we can't get the iolock just skip truncating the blocks
		 * past EOF because we could deadlock with the mmap_lock
		 * otherwise. We'll get another chance to drop them once the
		 * last reference to the inode is dropped, so we'll never leak
		 * blocks permanently.
		 */
		if (xfs_ilock_nowait(ip, XFS_IOLOCK_EXCL)) {
			error = xfs_free_eofblocks(ip);
			xfs_iunlock(ip, XFS_IOLOCK_EXCL);
			if (error)
				return error;
		}

		/* delalloc blocks after truncation means it really is dirty */
		if (ip->i_delayed_blks)
			xfs_iflags_set(ip, XFS_IDIRTY_RELEASE);
	}
	return 0;
}
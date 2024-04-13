xfs_inactive(
	xfs_inode_t	*ip)
{
	struct xfs_mount	*mp;
	int			error;
	int			truncate = 0;

	/*
	 * If the inode is already free, then there can be nothing
	 * to clean up here.
	 */
	if (VFS_I(ip)->i_mode == 0) {
		ASSERT(ip->i_df.if_broot_bytes == 0);
		return;
	}

	mp = ip->i_mount;
	ASSERT(!xfs_iflags_test(ip, XFS_IRECOVERY));

	/* If this is a read-only mount, don't do this (would generate I/O) */
	if (mp->m_flags & XFS_MOUNT_RDONLY)
		return;

	/* Try to clean out the cow blocks if there are any. */
	if (xfs_inode_has_cow_data(ip))
		xfs_reflink_cancel_cow_range(ip, 0, NULLFILEOFF, true);

	if (VFS_I(ip)->i_nlink != 0) {
		/*
		 * force is true because we are evicting an inode from the
		 * cache. Post-eof blocks must be freed, lest we end up with
		 * broken free space accounting.
		 *
		 * Note: don't bother with iolock here since lockdep complains
		 * about acquiring it in reclaim context. We have the only
		 * reference to the inode at this point anyways.
		 */
		if (xfs_can_free_eofblocks(ip, true))
			xfs_free_eofblocks(ip);

		return;
	}

	if (S_ISREG(VFS_I(ip)->i_mode) &&
	    (ip->i_d.di_size != 0 || XFS_ISIZE(ip) != 0 ||
	     ip->i_df.if_nextents > 0 || ip->i_delayed_blks > 0))
		truncate = 1;

	error = xfs_qm_dqattach(ip);
	if (error)
		return;

	if (S_ISLNK(VFS_I(ip)->i_mode))
		error = xfs_inactive_symlink(ip);
	else if (truncate)
		error = xfs_inactive_truncate(ip);
	if (error)
		return;

	/*
	 * If there are attributes associated with the file then blow them away
	 * now.  The code calls a routine that recursively deconstructs the
	 * attribute fork. If also blows away the in-core attribute fork.
	 */
	if (XFS_IFORK_Q(ip)) {
		error = xfs_attr_inactive(ip);
		if (error)
			return;
	}

	ASSERT(!ip->i_afp);
	ASSERT(ip->i_d.di_forkoff == 0);

	/*
	 * Free the inode.
	 */
	error = xfs_inactive_ifree(ip);
	if (error)
		return;

	/*
	 * Release the dquots held by inode, if any.
	 */
	xfs_qm_dqdetach(ip);
}
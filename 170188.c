xfs_itruncate_extents_flags(
	struct xfs_trans	**tpp,
	struct xfs_inode	*ip,
	int			whichfork,
	xfs_fsize_t		new_size,
	int			flags)
{
	struct xfs_mount	*mp = ip->i_mount;
	struct xfs_trans	*tp = *tpp;
	xfs_fileoff_t		first_unmap_block;
	xfs_filblks_t		unmap_len;
	int			error = 0;

	ASSERT(xfs_isilocked(ip, XFS_ILOCK_EXCL));
	ASSERT(!atomic_read(&VFS_I(ip)->i_count) ||
	       xfs_isilocked(ip, XFS_IOLOCK_EXCL));
	ASSERT(new_size <= XFS_ISIZE(ip));
	ASSERT(tp->t_flags & XFS_TRANS_PERM_LOG_RES);
	ASSERT(ip->i_itemp != NULL);
	ASSERT(ip->i_itemp->ili_lock_flags == 0);
	ASSERT(!XFS_NOT_DQATTACHED(mp, ip));

	trace_xfs_itruncate_extents_start(ip, new_size);

	flags |= xfs_bmapi_aflag(whichfork);

	/*
	 * Since it is possible for space to become allocated beyond
	 * the end of the file (in a crash where the space is allocated
	 * but the inode size is not yet updated), simply remove any
	 * blocks which show up between the new EOF and the maximum
	 * possible file size.
	 *
	 * We have to free all the blocks to the bmbt maximum offset, even if
	 * the page cache can't scale that far.
	 */
	first_unmap_block = XFS_B_TO_FSB(mp, (xfs_ufsize_t)new_size);
	if (!xfs_verify_fileoff(mp, first_unmap_block)) {
		WARN_ON_ONCE(first_unmap_block > XFS_MAX_FILEOFF);
		return 0;
	}

	unmap_len = XFS_MAX_FILEOFF - first_unmap_block + 1;
	while (unmap_len > 0) {
		ASSERT(tp->t_firstblock == NULLFSBLOCK);
		error = __xfs_bunmapi(tp, ip, first_unmap_block, &unmap_len,
				flags, XFS_ITRUNC_MAX_EXTENTS);
		if (error)
			goto out;

		/* free the just unmapped extents */
		error = xfs_defer_finish(&tp);
		if (error)
			goto out;
	}

	if (whichfork == XFS_DATA_FORK) {
		/* Remove all pending CoW reservations. */
		error = xfs_reflink_cancel_cow_blocks(ip, &tp,
				first_unmap_block, XFS_MAX_FILEOFF, true);
		if (error)
			goto out;

		xfs_itruncate_clear_reflink_flags(ip);
	}

	/*
	 * Always re-log the inode so that our permanent transaction can keep
	 * on rolling it forward in the log.
	 */
	xfs_trans_log_inode(tp, ip, XFS_ILOG_CORE);

	trace_xfs_itruncate_extents_end(ip, new_size);

out:
	*tpp = tp;
	return error;
}
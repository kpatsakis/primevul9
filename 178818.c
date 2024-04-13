xfs_bmapi_convert_unwritten(
	struct xfs_bmalloca	*bma,
	struct xfs_bmbt_irec	*mval,
	xfs_filblks_t		len,
	int			flags)
{
	int			whichfork = xfs_bmapi_whichfork(flags);
	struct xfs_ifork	*ifp = XFS_IFORK_PTR(bma->ip, whichfork);
	int			tmp_logflags = 0;
	int			error;

	/* check if we need to do unwritten->real conversion */
	if (mval->br_state == XFS_EXT_UNWRITTEN &&
	    (flags & XFS_BMAPI_PREALLOC))
		return 0;

	/* check if we need to do real->unwritten conversion */
	if (mval->br_state == XFS_EXT_NORM &&
	    (flags & (XFS_BMAPI_PREALLOC | XFS_BMAPI_CONVERT)) !=
			(XFS_BMAPI_PREALLOC | XFS_BMAPI_CONVERT))
		return 0;

	/*
	 * Modify (by adding) the state flag, if writing.
	 */
	ASSERT(mval->br_blockcount <= len);
	if ((ifp->if_flags & XFS_IFBROOT) && !bma->cur) {
		bma->cur = xfs_bmbt_init_cursor(bma->ip->i_mount, bma->tp,
					bma->ip, whichfork);
		bma->cur->bc_private.b.firstblock = *bma->firstblock;
		bma->cur->bc_private.b.dfops = bma->dfops;
	}
	mval->br_state = (mval->br_state == XFS_EXT_UNWRITTEN)
				? XFS_EXT_NORM : XFS_EXT_UNWRITTEN;

	/*
	 * Before insertion into the bmbt, zero the range being converted
	 * if required.
	 */
	if (flags & XFS_BMAPI_ZERO) {
		error = xfs_zero_extent(bma->ip, mval->br_startblock,
					mval->br_blockcount);
		if (error)
			return error;
	}

	error = xfs_bmap_add_extent_unwritten_real(bma->tp, bma->ip, whichfork,
			&bma->icur, &bma->cur, mval, bma->firstblock,
			bma->dfops, &tmp_logflags);
	/*
	 * Log the inode core unconditionally in the unwritten extent conversion
	 * path because the conversion might not have done so (e.g., if the
	 * extent count hasn't changed). We need to make sure the inode is dirty
	 * in the transaction for the sake of fsync(), even if nothing has
	 * changed, because fsync() will not force the log for this transaction
	 * unless it sees the inode pinned.
	 *
	 * Note: If we're only converting cow fork extents, there aren't
	 * any on-disk updates to make, so we don't need to log anything.
	 */
	if (whichfork != XFS_COW_FORK)
		bma->logflags |= tmp_logflags | XFS_ILOG_CORE;
	if (error)
		return error;

	/*
	 * Update our extent pointer, given that
	 * xfs_bmap_add_extent_unwritten_real might have merged it into one
	 * of the neighbouring ones.
	 */
	xfs_iext_get_extent(ifp, &bma->icur, &bma->got);

	/*
	 * We may have combined previously unwritten space with written space,
	 * so generate another request.
	 */
	if (mval->br_blockcount < len)
		return -EAGAIN;
	return 0;
}
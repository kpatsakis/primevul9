xfs_bmse_merge(
	struct xfs_inode		*ip,
	int				whichfork,
	xfs_fileoff_t			shift,		/* shift fsb */
	struct xfs_iext_cursor		*icur,
	struct xfs_bmbt_irec		*got,		/* extent to shift */
	struct xfs_bmbt_irec		*left,		/* preceding extent */
	struct xfs_btree_cur		*cur,
	int				*logflags,	/* output */
	struct xfs_defer_ops		*dfops)
{
	struct xfs_bmbt_irec		new;
	xfs_filblks_t			blockcount;
	int				error, i;
	struct xfs_mount		*mp = ip->i_mount;

	blockcount = left->br_blockcount + got->br_blockcount;

	ASSERT(xfs_isilocked(ip, XFS_IOLOCK_EXCL));
	ASSERT(xfs_isilocked(ip, XFS_ILOCK_EXCL));
	ASSERT(xfs_bmse_can_merge(left, got, shift));

	new = *left;
	new.br_blockcount = blockcount;

	/*
	 * Update the on-disk extent count, the btree if necessary and log the
	 * inode.
	 */
	XFS_IFORK_NEXT_SET(ip, whichfork,
			   XFS_IFORK_NEXTENTS(ip, whichfork) - 1);
	*logflags |= XFS_ILOG_CORE;
	if (!cur) {
		*logflags |= XFS_ILOG_DEXT;
		goto done;
	}

	/* lookup and remove the extent to merge */
	error = xfs_bmbt_lookup_eq(cur, got, &i);
	if (error)
		return error;
	XFS_WANT_CORRUPTED_RETURN(mp, i == 1);

	error = xfs_btree_delete(cur, &i);
	if (error)
		return error;
	XFS_WANT_CORRUPTED_RETURN(mp, i == 1);

	/* lookup and update size of the previous extent */
	error = xfs_bmbt_lookup_eq(cur, left, &i);
	if (error)
		return error;
	XFS_WANT_CORRUPTED_RETURN(mp, i == 1);

	error = xfs_bmbt_update(cur, &new);
	if (error)
		return error;

done:
	xfs_iext_remove(ip, icur, 0);
	xfs_iext_prev(XFS_IFORK_PTR(ip, whichfork), icur);
	xfs_iext_update_extent(ip, xfs_bmap_fork_to_state(whichfork), icur,
			&new);

	/* update reverse mapping. rmap functions merge the rmaps for us */
	error = xfs_rmap_unmap_extent(mp, dfops, ip, whichfork, got);
	if (error)
		return error;
	memcpy(&new, got, sizeof(new));
	new.br_startoff = left->br_startoff + left->br_blockcount;
	return xfs_rmap_map_extent(mp, dfops, ip, whichfork, &new);
}
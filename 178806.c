xfs_bmap_shift_update_extent(
	struct xfs_inode	*ip,
	int			whichfork,
	struct xfs_iext_cursor	*icur,
	struct xfs_bmbt_irec	*got,
	struct xfs_btree_cur	*cur,
	int			*logflags,
	struct xfs_defer_ops	*dfops,
	xfs_fileoff_t		startoff)
{
	struct xfs_mount	*mp = ip->i_mount;
	struct xfs_bmbt_irec	prev = *got;
	int			error, i;

	*logflags |= XFS_ILOG_CORE;

	got->br_startoff = startoff;

	if (cur) {
		error = xfs_bmbt_lookup_eq(cur, &prev, &i);
		if (error)
			return error;
		XFS_WANT_CORRUPTED_RETURN(mp, i == 1);

		error = xfs_bmbt_update(cur, got);
		if (error)
			return error;
	} else {
		*logflags |= XFS_ILOG_DEXT;
	}

	xfs_iext_update_extent(ip, xfs_bmap_fork_to_state(whichfork), icur,
			got);

	/* update reverse mapping */
	error = xfs_rmap_unmap_extent(mp, dfops, ip, whichfork, &prev);
	if (error)
		return error;
	return xfs_rmap_map_extent(mp, dfops, ip, whichfork, got);
}
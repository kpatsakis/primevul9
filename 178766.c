xfs_bmap_split_extent_at(
	struct xfs_trans	*tp,
	struct xfs_inode	*ip,
	xfs_fileoff_t		split_fsb,
	xfs_fsblock_t		*firstfsb,
	struct xfs_defer_ops	*dfops)
{
	int				whichfork = XFS_DATA_FORK;
	struct xfs_btree_cur		*cur = NULL;
	struct xfs_bmbt_irec		got;
	struct xfs_bmbt_irec		new; /* split extent */
	struct xfs_mount		*mp = ip->i_mount;
	struct xfs_ifork		*ifp;
	xfs_fsblock_t			gotblkcnt; /* new block count for got */
	struct xfs_iext_cursor		icur;
	int				error = 0;
	int				logflags = 0;
	int				i = 0;

	if (unlikely(XFS_TEST_ERROR(
	    (XFS_IFORK_FORMAT(ip, whichfork) != XFS_DINODE_FMT_EXTENTS &&
	     XFS_IFORK_FORMAT(ip, whichfork) != XFS_DINODE_FMT_BTREE),
	     mp, XFS_ERRTAG_BMAPIFORMAT))) {
		XFS_ERROR_REPORT("xfs_bmap_split_extent_at",
				 XFS_ERRLEVEL_LOW, mp);
		return -EFSCORRUPTED;
	}

	if (XFS_FORCED_SHUTDOWN(mp))
		return -EIO;

	ifp = XFS_IFORK_PTR(ip, whichfork);
	if (!(ifp->if_flags & XFS_IFEXTENTS)) {
		/* Read in all the extents */
		error = xfs_iread_extents(tp, ip, whichfork);
		if (error)
			return error;
	}

	/*
	 * If there are not extents, or split_fsb lies in a hole we are done.
	 */
	if (!xfs_iext_lookup_extent(ip, ifp, split_fsb, &icur, &got) ||
	    got.br_startoff >= split_fsb)
		return 0;

	gotblkcnt = split_fsb - got.br_startoff;
	new.br_startoff = split_fsb;
	new.br_startblock = got.br_startblock + gotblkcnt;
	new.br_blockcount = got.br_blockcount - gotblkcnt;
	new.br_state = got.br_state;

	if (ifp->if_flags & XFS_IFBROOT) {
		cur = xfs_bmbt_init_cursor(mp, tp, ip, whichfork);
		cur->bc_private.b.firstblock = *firstfsb;
		cur->bc_private.b.dfops = dfops;
		cur->bc_private.b.flags = 0;
		error = xfs_bmbt_lookup_eq(cur, &got, &i);
		if (error)
			goto del_cursor;
		XFS_WANT_CORRUPTED_GOTO(mp, i == 1, del_cursor);
	}

	got.br_blockcount = gotblkcnt;
	xfs_iext_update_extent(ip, xfs_bmap_fork_to_state(whichfork), &icur,
			&got);

	logflags = XFS_ILOG_CORE;
	if (cur) {
		error = xfs_bmbt_update(cur, &got);
		if (error)
			goto del_cursor;
	} else
		logflags |= XFS_ILOG_DEXT;

	/* Add new extent */
	xfs_iext_next(ifp, &icur);
	xfs_iext_insert(ip, &icur, &new, 0);
	XFS_IFORK_NEXT_SET(ip, whichfork,
			   XFS_IFORK_NEXTENTS(ip, whichfork) + 1);

	if (cur) {
		error = xfs_bmbt_lookup_eq(cur, &new, &i);
		if (error)
			goto del_cursor;
		XFS_WANT_CORRUPTED_GOTO(mp, i == 0, del_cursor);
		error = xfs_btree_insert(cur, &i);
		if (error)
			goto del_cursor;
		XFS_WANT_CORRUPTED_GOTO(mp, i == 1, del_cursor);
	}

	/*
	 * Convert to a btree if necessary.
	 */
	if (xfs_bmap_needs_btree(ip, whichfork)) {
		int tmp_logflags; /* partial log flag return val */

		ASSERT(cur == NULL);
		error = xfs_bmap_extents_to_btree(tp, ip, firstfsb, dfops,
				&cur, 0, &tmp_logflags, whichfork);
		logflags |= tmp_logflags;
	}

del_cursor:
	if (cur) {
		cur->bc_private.b.allocated = 0;
		xfs_btree_del_cursor(cur,
				error ? XFS_BTREE_ERROR : XFS_BTREE_NOERROR);
	}

	if (logflags)
		xfs_trans_log_inode(tp, ip, logflags);
	return error;
}
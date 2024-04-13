xfs_bmap_collapse_extents(
	struct xfs_trans	*tp,
	struct xfs_inode	*ip,
	xfs_fileoff_t		*next_fsb,
	xfs_fileoff_t		offset_shift_fsb,
	bool			*done,
	xfs_fsblock_t		*firstblock,
	struct xfs_defer_ops	*dfops)
{
	int			whichfork = XFS_DATA_FORK;
	struct xfs_mount	*mp = ip->i_mount;
	struct xfs_ifork	*ifp = XFS_IFORK_PTR(ip, whichfork);
	struct xfs_btree_cur	*cur = NULL;
	struct xfs_bmbt_irec	got, prev;
	struct xfs_iext_cursor	icur;
	xfs_fileoff_t		new_startoff;
	int			error = 0;
	int			logflags = 0;

	if (unlikely(XFS_TEST_ERROR(
	    (XFS_IFORK_FORMAT(ip, whichfork) != XFS_DINODE_FMT_EXTENTS &&
	     XFS_IFORK_FORMAT(ip, whichfork) != XFS_DINODE_FMT_BTREE),
	     mp, XFS_ERRTAG_BMAPIFORMAT))) {
		XFS_ERROR_REPORT(__func__, XFS_ERRLEVEL_LOW, mp);
		return -EFSCORRUPTED;
	}

	if (XFS_FORCED_SHUTDOWN(mp))
		return -EIO;

	ASSERT(xfs_isilocked(ip, XFS_IOLOCK_EXCL | XFS_ILOCK_EXCL));

	if (!(ifp->if_flags & XFS_IFEXTENTS)) {
		error = xfs_iread_extents(tp, ip, whichfork);
		if (error)
			return error;
	}

	if (ifp->if_flags & XFS_IFBROOT) {
		cur = xfs_bmbt_init_cursor(mp, tp, ip, whichfork);
		cur->bc_private.b.firstblock = *firstblock;
		cur->bc_private.b.dfops = dfops;
		cur->bc_private.b.flags = 0;
	}

	if (!xfs_iext_lookup_extent(ip, ifp, *next_fsb, &icur, &got)) {
		*done = true;
		goto del_cursor;
	}
	XFS_WANT_CORRUPTED_GOTO(mp, !isnullstartblock(got.br_startblock),
				del_cursor);

	new_startoff = got.br_startoff - offset_shift_fsb;
	if (xfs_iext_peek_prev_extent(ifp, &icur, &prev)) {
		if (new_startoff < prev.br_startoff + prev.br_blockcount) {
			error = -EINVAL;
			goto del_cursor;
		}

		if (xfs_bmse_can_merge(&prev, &got, offset_shift_fsb)) {
			error = xfs_bmse_merge(ip, whichfork, offset_shift_fsb,
					&icur, &got, &prev, cur, &logflags,
					dfops);
			if (error)
				goto del_cursor;
			goto done;
		}
	} else {
		if (got.br_startoff < offset_shift_fsb) {
			error = -EINVAL;
			goto del_cursor;
		}
	}

	error = xfs_bmap_shift_update_extent(ip, whichfork, &icur, &got, cur,
			&logflags, dfops, new_startoff);
	if (error)
		goto del_cursor;

done:
	if (!xfs_iext_next_extent(ifp, &icur, &got)) {
		*done = true;
		goto del_cursor;
	}

	*next_fsb = got.br_startoff;
del_cursor:
	if (cur)
		xfs_btree_del_cursor(cur,
			error ? XFS_BTREE_ERROR : XFS_BTREE_NOERROR);
	if (logflags)
		xfs_trans_log_inode(tp, ip, logflags);
	return error;
}
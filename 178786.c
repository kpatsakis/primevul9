xfs_bmap_insert_extents(
	struct xfs_trans	*tp,
	struct xfs_inode	*ip,
	xfs_fileoff_t		*next_fsb,
	xfs_fileoff_t		offset_shift_fsb,
	bool			*done,
	xfs_fileoff_t		stop_fsb,
	xfs_fsblock_t		*firstblock,
	struct xfs_defer_ops	*dfops)
{
	int			whichfork = XFS_DATA_FORK;
	struct xfs_mount	*mp = ip->i_mount;
	struct xfs_ifork	*ifp = XFS_IFORK_PTR(ip, whichfork);
	struct xfs_btree_cur	*cur = NULL;
	struct xfs_bmbt_irec	got, next;
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

	if (*next_fsb == NULLFSBLOCK) {
		xfs_iext_last(ifp, &icur);
		if (!xfs_iext_get_extent(ifp, &icur, &got) ||
		    stop_fsb > got.br_startoff) {
			*done = true;
			goto del_cursor;
		}
	} else {
		if (!xfs_iext_lookup_extent(ip, ifp, *next_fsb, &icur, &got)) {
			*done = true;
			goto del_cursor;
		}
	}
	XFS_WANT_CORRUPTED_GOTO(mp, !isnullstartblock(got.br_startblock),
				del_cursor);

	if (stop_fsb >= got.br_startoff + got.br_blockcount) {
		error = -EIO;
		goto del_cursor;
	}

	new_startoff = got.br_startoff + offset_shift_fsb;
	if (xfs_iext_peek_next_extent(ifp, &icur, &next)) {
		if (new_startoff + got.br_blockcount > next.br_startoff) {
			error = -EINVAL;
			goto del_cursor;
		}

		/*
		 * Unlike a left shift (which involves a hole punch), a right
		 * shift does not modify extent neighbors in any way.  We should
		 * never find mergeable extents in this scenario.  Check anyways
		 * and warn if we encounter two extents that could be one.
		 */
		if (xfs_bmse_can_merge(&got, &next, offset_shift_fsb))
			WARN_ON_ONCE(1);
	}

	error = xfs_bmap_shift_update_extent(ip, whichfork, &icur, &got, cur,
			&logflags, dfops, new_startoff);
	if (error)
		goto del_cursor;

	if (!xfs_iext_prev_extent(ifp, &icur, &got) ||
	    stop_fsb >= got.br_startoff + got.br_blockcount) {
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
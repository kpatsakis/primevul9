xfs_bmapi_remap(
	struct xfs_trans	*tp,
	struct xfs_inode	*ip,
	xfs_fileoff_t		bno,
	xfs_filblks_t		len,
	xfs_fsblock_t		startblock,
	struct xfs_defer_ops	*dfops)
{
	struct xfs_mount	*mp = ip->i_mount;
	struct xfs_ifork	*ifp = XFS_IFORK_PTR(ip, XFS_DATA_FORK);
	struct xfs_btree_cur	*cur = NULL;
	xfs_fsblock_t		firstblock = NULLFSBLOCK;
	struct xfs_bmbt_irec	got;
	struct xfs_iext_cursor	icur;
	int			logflags = 0, error;

	ASSERT(len > 0);
	ASSERT(len <= (xfs_filblks_t)MAXEXTLEN);
	ASSERT(xfs_isilocked(ip, XFS_ILOCK_EXCL));

	if (unlikely(XFS_TEST_ERROR(
	    (XFS_IFORK_FORMAT(ip, XFS_DATA_FORK) != XFS_DINODE_FMT_EXTENTS &&
	     XFS_IFORK_FORMAT(ip, XFS_DATA_FORK) != XFS_DINODE_FMT_BTREE),
	     mp, XFS_ERRTAG_BMAPIFORMAT))) {
		XFS_ERROR_REPORT("xfs_bmapi_remap", XFS_ERRLEVEL_LOW, mp);
		return -EFSCORRUPTED;
	}

	if (XFS_FORCED_SHUTDOWN(mp))
		return -EIO;

	if (!(ifp->if_flags & XFS_IFEXTENTS)) {
		error = xfs_iread_extents(NULL, ip, XFS_DATA_FORK);
		if (error)
			return error;
	}

	if (xfs_iext_lookup_extent(ip, ifp, bno, &icur, &got)) {
		/* make sure we only reflink into a hole. */
		ASSERT(got.br_startoff > bno);
		ASSERT(got.br_startoff - bno >= len);
	}

	ip->i_d.di_nblocks += len;
	xfs_trans_log_inode(tp, ip, XFS_ILOG_CORE);

	if (ifp->if_flags & XFS_IFBROOT) {
		cur = xfs_bmbt_init_cursor(mp, tp, ip, XFS_DATA_FORK);
		cur->bc_private.b.firstblock = firstblock;
		cur->bc_private.b.dfops = dfops;
		cur->bc_private.b.flags = 0;
	}

	got.br_startoff = bno;
	got.br_startblock = startblock;
	got.br_blockcount = len;
	got.br_state = XFS_EXT_NORM;

	error = xfs_bmap_add_extent_hole_real(tp, ip, XFS_DATA_FORK, &icur,
			&cur, &got, &firstblock, dfops, &logflags);
	if (error)
		goto error0;

	if (xfs_bmap_wants_extents(ip, XFS_DATA_FORK)) {
		int		tmp_logflags = 0;

		error = xfs_bmap_btree_to_extents(tp, ip, cur,
			&tmp_logflags, XFS_DATA_FORK);
		logflags |= tmp_logflags;
	}

error0:
	if (ip->i_d.di_format != XFS_DINODE_FMT_EXTENTS)
		logflags &= ~XFS_ILOG_DEXT;
	else if (ip->i_d.di_format != XFS_DINODE_FMT_BTREE)
		logflags &= ~XFS_ILOG_DBROOT;

	if (logflags)
		xfs_trans_log_inode(tp, ip, logflags);
	if (cur) {
		xfs_btree_del_cursor(cur,
				error ? XFS_BTREE_ERROR : XFS_BTREE_NOERROR);
	}
	return error;
}
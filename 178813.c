xfs_bmapi_read(
	struct xfs_inode	*ip,
	xfs_fileoff_t		bno,
	xfs_filblks_t		len,
	struct xfs_bmbt_irec	*mval,
	int			*nmap,
	int			flags)
{
	struct xfs_mount	*mp = ip->i_mount;
	struct xfs_ifork	*ifp;
	struct xfs_bmbt_irec	got;
	xfs_fileoff_t		obno;
	xfs_fileoff_t		end;
	struct xfs_iext_cursor	icur;
	int			error;
	bool			eof = false;
	int			n = 0;
	int			whichfork = xfs_bmapi_whichfork(flags);

	ASSERT(*nmap >= 1);
	ASSERT(!(flags & ~(XFS_BMAPI_ATTRFORK|XFS_BMAPI_ENTIRE|
			   XFS_BMAPI_IGSTATE|XFS_BMAPI_COWFORK)));
	ASSERT(xfs_isilocked(ip, XFS_ILOCK_SHARED|XFS_ILOCK_EXCL));

	if (unlikely(XFS_TEST_ERROR(
	    (XFS_IFORK_FORMAT(ip, whichfork) != XFS_DINODE_FMT_EXTENTS &&
	     XFS_IFORK_FORMAT(ip, whichfork) != XFS_DINODE_FMT_BTREE),
	     mp, XFS_ERRTAG_BMAPIFORMAT))) {
		XFS_ERROR_REPORT("xfs_bmapi_read", XFS_ERRLEVEL_LOW, mp);
		return -EFSCORRUPTED;
	}

	if (XFS_FORCED_SHUTDOWN(mp))
		return -EIO;

	XFS_STATS_INC(mp, xs_blk_mapr);

	ifp = XFS_IFORK_PTR(ip, whichfork);

	/* No CoW fork?  Return a hole. */
	if (whichfork == XFS_COW_FORK && !ifp) {
		mval->br_startoff = bno;
		mval->br_startblock = HOLESTARTBLOCK;
		mval->br_blockcount = len;
		mval->br_state = XFS_EXT_NORM;
		*nmap = 1;
		return 0;
	}

	if (!(ifp->if_flags & XFS_IFEXTENTS)) {
		error = xfs_iread_extents(NULL, ip, whichfork);
		if (error)
			return error;
	}

	if (!xfs_iext_lookup_extent(ip, ifp, bno, &icur, &got))
		eof = true;
	end = bno + len;
	obno = bno;

	while (bno < end && n < *nmap) {
		/* Reading past eof, act as though there's a hole up to end. */
		if (eof)
			got.br_startoff = end;
		if (got.br_startoff > bno) {
			/* Reading in a hole.  */
			mval->br_startoff = bno;
			mval->br_startblock = HOLESTARTBLOCK;
			mval->br_blockcount =
				XFS_FILBLKS_MIN(len, got.br_startoff - bno);
			mval->br_state = XFS_EXT_NORM;
			bno += mval->br_blockcount;
			len -= mval->br_blockcount;
			mval++;
			n++;
			continue;
		}

		/* set up the extent map to return. */
		xfs_bmapi_trim_map(mval, &got, &bno, len, obno, end, n, flags);
		xfs_bmapi_update_map(&mval, &bno, &len, obno, end, &n, flags);

		/* If we're done, stop now. */
		if (bno >= end || n >= *nmap)
			break;

		/* Else go on to the next record. */
		if (!xfs_iext_next_extent(ifp, &icur, &got))
			eof = true;
	}
	*nmap = n;
	return 0;
}
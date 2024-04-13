xfs_bmapi_write(
	struct xfs_trans	*tp,		/* transaction pointer */
	struct xfs_inode	*ip,		/* incore inode */
	xfs_fileoff_t		bno,		/* starting file offs. mapped */
	xfs_filblks_t		len,		/* length to map in file */
	int			flags,		/* XFS_BMAPI_... */
	xfs_fsblock_t		*firstblock,	/* first allocated block
						   controls a.g. for allocs */
	xfs_extlen_t		total,		/* total blocks needed */
	struct xfs_bmbt_irec	*mval,		/* output: map values */
	int			*nmap,		/* i/o: mval size/count */
	struct xfs_defer_ops	*dfops)		/* i/o: list extents to free */
{
	struct xfs_mount	*mp = ip->i_mount;
	struct xfs_ifork	*ifp;
	struct xfs_bmalloca	bma = { NULL };	/* args for xfs_bmap_alloc */
	xfs_fileoff_t		end;		/* end of mapped file region */
	bool			eof = false;	/* after the end of extents */
	int			error;		/* error return */
	int			n;		/* current extent index */
	xfs_fileoff_t		obno;		/* old block number (offset) */
	int			whichfork;	/* data or attr fork */

#ifdef DEBUG
	xfs_fileoff_t		orig_bno;	/* original block number value */
	int			orig_flags;	/* original flags arg value */
	xfs_filblks_t		orig_len;	/* original value of len arg */
	struct xfs_bmbt_irec	*orig_mval;	/* original value of mval */
	int			orig_nmap;	/* original value of *nmap */

	orig_bno = bno;
	orig_len = len;
	orig_flags = flags;
	orig_mval = mval;
	orig_nmap = *nmap;
#endif
	whichfork = xfs_bmapi_whichfork(flags);

	ASSERT(*nmap >= 1);
	ASSERT(*nmap <= XFS_BMAP_MAX_NMAP);
	ASSERT(!(flags & XFS_BMAPI_IGSTATE));
	ASSERT(tp != NULL ||
	       (flags & (XFS_BMAPI_CONVERT | XFS_BMAPI_COWFORK)) ==
			(XFS_BMAPI_CONVERT | XFS_BMAPI_COWFORK));
	ASSERT(len > 0);
	ASSERT(XFS_IFORK_FORMAT(ip, whichfork) != XFS_DINODE_FMT_LOCAL);
	ASSERT(xfs_isilocked(ip, XFS_ILOCK_EXCL));
	ASSERT(!(flags & XFS_BMAPI_REMAP));

	/* zeroing is for currently only for data extents, not metadata */
	ASSERT((flags & (XFS_BMAPI_METADATA | XFS_BMAPI_ZERO)) !=
			(XFS_BMAPI_METADATA | XFS_BMAPI_ZERO));
	/*
	 * we can allocate unwritten extents or pre-zero allocated blocks,
	 * but it makes no sense to do both at once. This would result in
	 * zeroing the unwritten extent twice, but it still being an
	 * unwritten extent....
	 */
	ASSERT((flags & (XFS_BMAPI_PREALLOC | XFS_BMAPI_ZERO)) !=
			(XFS_BMAPI_PREALLOC | XFS_BMAPI_ZERO));

	if (unlikely(XFS_TEST_ERROR(
	    (XFS_IFORK_FORMAT(ip, whichfork) != XFS_DINODE_FMT_EXTENTS &&
	     XFS_IFORK_FORMAT(ip, whichfork) != XFS_DINODE_FMT_BTREE),
	     mp, XFS_ERRTAG_BMAPIFORMAT))) {
		XFS_ERROR_REPORT("xfs_bmapi_write", XFS_ERRLEVEL_LOW, mp);
		return -EFSCORRUPTED;
	}

	if (XFS_FORCED_SHUTDOWN(mp))
		return -EIO;

	ifp = XFS_IFORK_PTR(ip, whichfork);

	XFS_STATS_INC(mp, xs_blk_mapw);

	if (*firstblock == NULLFSBLOCK) {
		if (XFS_IFORK_FORMAT(ip, whichfork) == XFS_DINODE_FMT_BTREE)
			bma.minleft = be16_to_cpu(ifp->if_broot->bb_level) + 1;
		else
			bma.minleft = 1;
	} else {
		bma.minleft = 0;
	}

	if (!(ifp->if_flags & XFS_IFEXTENTS)) {
		error = xfs_iread_extents(tp, ip, whichfork);
		if (error)
			goto error0;
	}

	n = 0;
	end = bno + len;
	obno = bno;

	if (!xfs_iext_lookup_extent(ip, ifp, bno, &bma.icur, &bma.got))
		eof = true;
	if (!xfs_iext_peek_prev_extent(ifp, &bma.icur, &bma.prev))
		bma.prev.br_startoff = NULLFILEOFF;
	bma.tp = tp;
	bma.ip = ip;
	bma.total = total;
	bma.datatype = 0;
	bma.dfops = dfops;
	bma.firstblock = firstblock;

	while (bno < end && n < *nmap) {
		bool			need_alloc = false, wasdelay = false;

		/* in hole or beyond EOF? */
		if (eof || bma.got.br_startoff > bno) {
			/*
			 * CoW fork conversions should /never/ hit EOF or
			 * holes.  There should always be something for us
			 * to work on.
			 */
			ASSERT(!((flags & XFS_BMAPI_CONVERT) &&
			         (flags & XFS_BMAPI_COWFORK)));

			if (flags & XFS_BMAPI_DELALLOC) {
				/*
				 * For the COW fork we can reasonably get a
				 * request for converting an extent that races
				 * with other threads already having converted
				 * part of it, as there converting COW to
				 * regular blocks is not protected using the
				 * IOLOCK.
				 */
				ASSERT(flags & XFS_BMAPI_COWFORK);
				if (!(flags & XFS_BMAPI_COWFORK)) {
					error = -EIO;
					goto error0;
				}

				if (eof || bno >= end)
					break;
			} else {
				need_alloc = true;
			}
		} else if (isnullstartblock(bma.got.br_startblock)) {
			wasdelay = true;
		}

		/*
		 * First, deal with the hole before the allocated space
		 * that we found, if any.
		 */
		if ((need_alloc || wasdelay) &&
		    !(flags & XFS_BMAPI_CONVERT_ONLY)) {
			bma.eof = eof;
			bma.conv = !!(flags & XFS_BMAPI_CONVERT);
			bma.wasdel = wasdelay;
			bma.offset = bno;
			bma.flags = flags;

			/*
			 * There's a 32/64 bit type mismatch between the
			 * allocation length request (which can be 64 bits in
			 * length) and the bma length request, which is
			 * xfs_extlen_t and therefore 32 bits. Hence we have to
			 * check for 32-bit overflows and handle them here.
			 */
			if (len > (xfs_filblks_t)MAXEXTLEN)
				bma.length = MAXEXTLEN;
			else
				bma.length = len;

			ASSERT(len > 0);
			ASSERT(bma.length > 0);
			error = xfs_bmapi_allocate(&bma);
			if (error)
				goto error0;
			if (bma.blkno == NULLFSBLOCK)
				break;

			/*
			 * If this is a CoW allocation, record the data in
			 * the refcount btree for orphan recovery.
			 */
			if (whichfork == XFS_COW_FORK) {
				error = xfs_refcount_alloc_cow_extent(mp, dfops,
						bma.blkno, bma.length);
				if (error)
					goto error0;
			}
		}

		/* Deal with the allocated space we found.  */
		xfs_bmapi_trim_map(mval, &bma.got, &bno, len, obno,
							end, n, flags);

		/* Execute unwritten extent conversion if necessary */
		error = xfs_bmapi_convert_unwritten(&bma, mval, len, flags);
		if (error == -EAGAIN)
			continue;
		if (error)
			goto error0;

		/* update the extent map to return */
		xfs_bmapi_update_map(&mval, &bno, &len, obno, end, &n, flags);

		/*
		 * If we're done, stop now.  Stop when we've allocated
		 * XFS_BMAP_MAX_NMAP extents no matter what.  Otherwise
		 * the transaction may get too big.
		 */
		if (bno >= end || n >= *nmap || bma.nallocs >= *nmap)
			break;

		/* Else go on to the next record. */
		bma.prev = bma.got;
		if (!xfs_iext_next_extent(ifp, &bma.icur, &bma.got))
			eof = true;
	}
	*nmap = n;

	/*
	 * Transform from btree to extents, give it cur.
	 */
	if (xfs_bmap_wants_extents(ip, whichfork)) {
		int		tmp_logflags = 0;

		ASSERT(bma.cur);
		error = xfs_bmap_btree_to_extents(tp, ip, bma.cur,
			&tmp_logflags, whichfork);
		bma.logflags |= tmp_logflags;
		if (error)
			goto error0;
	}

	ASSERT(XFS_IFORK_FORMAT(ip, whichfork) != XFS_DINODE_FMT_BTREE ||
	       XFS_IFORK_NEXTENTS(ip, whichfork) >
		XFS_IFORK_MAXEXT(ip, whichfork));
	error = 0;
error0:
	/*
	 * Log everything.  Do this after conversion, there's no point in
	 * logging the extent records if we've converted to btree format.
	 */
	if ((bma.logflags & xfs_ilog_fext(whichfork)) &&
	    XFS_IFORK_FORMAT(ip, whichfork) != XFS_DINODE_FMT_EXTENTS)
		bma.logflags &= ~xfs_ilog_fext(whichfork);
	else if ((bma.logflags & xfs_ilog_fbroot(whichfork)) &&
		 XFS_IFORK_FORMAT(ip, whichfork) != XFS_DINODE_FMT_BTREE)
		bma.logflags &= ~xfs_ilog_fbroot(whichfork);
	/*
	 * Log whatever the flags say, even if error.  Otherwise we might miss
	 * detecting a case where the data is changed, there's an error,
	 * and it's not logged so we don't shutdown when we should.
	 */
	if (bma.logflags)
		xfs_trans_log_inode(tp, ip, bma.logflags);

	if (bma.cur) {
		if (!error) {
			ASSERT(*firstblock == NULLFSBLOCK ||
			       XFS_FSB_TO_AGNO(mp, *firstblock) <=
			       XFS_FSB_TO_AGNO(mp,
				       bma.cur->bc_private.b.firstblock));
			*firstblock = bma.cur->bc_private.b.firstblock;
		}
		xfs_btree_del_cursor(bma.cur,
			error ? XFS_BTREE_ERROR : XFS_BTREE_NOERROR);
	}
	if (!error)
		xfs_bmap_validate_ret(orig_bno, orig_len, orig_flags, orig_mval,
			orig_nmap, *nmap);
	return error;
}
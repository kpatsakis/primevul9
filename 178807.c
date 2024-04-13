__xfs_bunmapi(
	xfs_trans_t		*tp,		/* transaction pointer */
	struct xfs_inode	*ip,		/* incore inode */
	xfs_fileoff_t		start,		/* first file offset deleted */
	xfs_filblks_t		*rlen,		/* i/o: amount remaining */
	int			flags,		/* misc flags */
	xfs_extnum_t		nexts,		/* number of extents max */
	xfs_fsblock_t		*firstblock,	/* first allocated block
						   controls a.g. for allocs */
	struct xfs_defer_ops	*dfops)		/* i/o: deferred updates */
{
	xfs_btree_cur_t		*cur;		/* bmap btree cursor */
	xfs_bmbt_irec_t		del;		/* extent being deleted */
	int			error;		/* error return value */
	xfs_extnum_t		extno;		/* extent number in list */
	xfs_bmbt_irec_t		got;		/* current extent record */
	xfs_ifork_t		*ifp;		/* inode fork pointer */
	int			isrt;		/* freeing in rt area */
	int			logflags;	/* transaction logging flags */
	xfs_extlen_t		mod;		/* rt extent offset */
	xfs_mount_t		*mp;		/* mount structure */
	int			tmp_logflags;	/* partial logging flags */
	int			wasdel;		/* was a delayed alloc extent */
	int			whichfork;	/* data or attribute fork */
	xfs_fsblock_t		sum;
	xfs_filblks_t		len = *rlen;	/* length to unmap in file */
	xfs_fileoff_t		max_len;
	xfs_agnumber_t		prev_agno = NULLAGNUMBER, agno;
	xfs_fileoff_t		end;
	struct xfs_iext_cursor	icur;
	bool			done = false;

	trace_xfs_bunmap(ip, start, len, flags, _RET_IP_);

	whichfork = xfs_bmapi_whichfork(flags);
	ASSERT(whichfork != XFS_COW_FORK);
	ifp = XFS_IFORK_PTR(ip, whichfork);
	if (unlikely(
	    XFS_IFORK_FORMAT(ip, whichfork) != XFS_DINODE_FMT_EXTENTS &&
	    XFS_IFORK_FORMAT(ip, whichfork) != XFS_DINODE_FMT_BTREE)) {
		XFS_ERROR_REPORT("xfs_bunmapi", XFS_ERRLEVEL_LOW,
				 ip->i_mount);
		return -EFSCORRUPTED;
	}
	mp = ip->i_mount;
	if (XFS_FORCED_SHUTDOWN(mp))
		return -EIO;

	ASSERT(xfs_isilocked(ip, XFS_ILOCK_EXCL));
	ASSERT(len > 0);
	ASSERT(nexts >= 0);

	/*
	 * Guesstimate how many blocks we can unmap without running the risk of
	 * blowing out the transaction with a mix of EFIs and reflink
	 * adjustments.
	 */
	if (tp && xfs_is_reflink_inode(ip) && whichfork == XFS_DATA_FORK)
		max_len = min(len, xfs_refcount_max_unmap(tp->t_log_res));
	else
		max_len = len;

	if (!(ifp->if_flags & XFS_IFEXTENTS) &&
	    (error = xfs_iread_extents(tp, ip, whichfork)))
		return error;
	if (xfs_iext_count(ifp) == 0) {
		*rlen = 0;
		return 0;
	}
	XFS_STATS_INC(mp, xs_blk_unmap);
	isrt = (whichfork == XFS_DATA_FORK) && XFS_IS_REALTIME_INODE(ip);
	end = start + len;

	if (!xfs_iext_lookup_extent_before(ip, ifp, &end, &icur, &got)) {
		*rlen = 0;
		return 0;
	}
	end--;

	logflags = 0;
	if (ifp->if_flags & XFS_IFBROOT) {
		ASSERT(XFS_IFORK_FORMAT(ip, whichfork) == XFS_DINODE_FMT_BTREE);
		cur = xfs_bmbt_init_cursor(mp, tp, ip, whichfork);
		cur->bc_private.b.firstblock = *firstblock;
		cur->bc_private.b.dfops = dfops;
		cur->bc_private.b.flags = 0;
	} else
		cur = NULL;

	if (isrt) {
		/*
		 * Synchronize by locking the bitmap inode.
		 */
		xfs_ilock(mp->m_rbmip, XFS_ILOCK_EXCL|XFS_ILOCK_RTBITMAP);
		xfs_trans_ijoin(tp, mp->m_rbmip, XFS_ILOCK_EXCL);
		xfs_ilock(mp->m_rsumip, XFS_ILOCK_EXCL|XFS_ILOCK_RTSUM);
		xfs_trans_ijoin(tp, mp->m_rsumip, XFS_ILOCK_EXCL);
	}

	extno = 0;
	while (end != (xfs_fileoff_t)-1 && end >= start &&
	       (nexts == 0 || extno < nexts) && max_len > 0) {
		/*
		 * Is the found extent after a hole in which end lives?
		 * Just back up to the previous extent, if so.
		 */
		if (got.br_startoff > end &&
		    !xfs_iext_prev_extent(ifp, &icur, &got)) {
			done = true;
			break;
		}
		/*
		 * Is the last block of this extent before the range
		 * we're supposed to delete?  If so, we're done.
		 */
		end = XFS_FILEOFF_MIN(end,
			got.br_startoff + got.br_blockcount - 1);
		if (end < start)
			break;
		/*
		 * Then deal with the (possibly delayed) allocated space
		 * we found.
		 */
		del = got;
		wasdel = isnullstartblock(del.br_startblock);

		/*
		 * Make sure we don't touch multiple AGF headers out of order
		 * in a single transaction, as that could cause AB-BA deadlocks.
		 */
		if (!wasdel) {
			agno = XFS_FSB_TO_AGNO(mp, del.br_startblock);
			if (prev_agno != NULLAGNUMBER && prev_agno > agno)
				break;
			prev_agno = agno;
		}
		if (got.br_startoff < start) {
			del.br_startoff = start;
			del.br_blockcount -= start - got.br_startoff;
			if (!wasdel)
				del.br_startblock += start - got.br_startoff;
		}
		if (del.br_startoff + del.br_blockcount > end + 1)
			del.br_blockcount = end + 1 - del.br_startoff;

		/* How much can we safely unmap? */
		if (max_len < del.br_blockcount) {
			del.br_startoff += del.br_blockcount - max_len;
			if (!wasdel)
				del.br_startblock += del.br_blockcount - max_len;
			del.br_blockcount = max_len;
		}

		sum = del.br_startblock + del.br_blockcount;
		if (isrt &&
		    (mod = do_mod(sum, mp->m_sb.sb_rextsize))) {
			/*
			 * Realtime extent not lined up at the end.
			 * The extent could have been split into written
			 * and unwritten pieces, or we could just be
			 * unmapping part of it.  But we can't really
			 * get rid of part of a realtime extent.
			 */
			if (del.br_state == XFS_EXT_UNWRITTEN ||
			    !xfs_sb_version_hasextflgbit(&mp->m_sb)) {
				/*
				 * This piece is unwritten, or we're not
				 * using unwritten extents.  Skip over it.
				 */
				ASSERT(end >= mod);
				end -= mod > del.br_blockcount ?
					del.br_blockcount : mod;
				if (end < got.br_startoff &&
				    !xfs_iext_prev_extent(ifp, &icur, &got)) {
					done = true;
					break;
				}
				continue;
			}
			/*
			 * It's written, turn it unwritten.
			 * This is better than zeroing it.
			 */
			ASSERT(del.br_state == XFS_EXT_NORM);
			ASSERT(tp->t_blk_res > 0);
			/*
			 * If this spans a realtime extent boundary,
			 * chop it back to the start of the one we end at.
			 */
			if (del.br_blockcount > mod) {
				del.br_startoff += del.br_blockcount - mod;
				del.br_startblock += del.br_blockcount - mod;
				del.br_blockcount = mod;
			}
			del.br_state = XFS_EXT_UNWRITTEN;
			error = xfs_bmap_add_extent_unwritten_real(tp, ip,
					whichfork, &icur, &cur, &del,
					firstblock, dfops, &logflags);
			if (error)
				goto error0;
			goto nodelete;
		}
		if (isrt && (mod = do_mod(del.br_startblock, mp->m_sb.sb_rextsize))) {
			/*
			 * Realtime extent is lined up at the end but not
			 * at the front.  We'll get rid of full extents if
			 * we can.
			 */
			mod = mp->m_sb.sb_rextsize - mod;
			if (del.br_blockcount > mod) {
				del.br_blockcount -= mod;
				del.br_startoff += mod;
				del.br_startblock += mod;
			} else if ((del.br_startoff == start &&
				    (del.br_state == XFS_EXT_UNWRITTEN ||
				     tp->t_blk_res == 0)) ||
				   !xfs_sb_version_hasextflgbit(&mp->m_sb)) {
				/*
				 * Can't make it unwritten.  There isn't
				 * a full extent here so just skip it.
				 */
				ASSERT(end >= del.br_blockcount);
				end -= del.br_blockcount;
				if (got.br_startoff > end &&
				    !xfs_iext_prev_extent(ifp, &icur, &got)) {
					done = true;
					break;
				}
				continue;
			} else if (del.br_state == XFS_EXT_UNWRITTEN) {
				struct xfs_bmbt_irec	prev;

				/*
				 * This one is already unwritten.
				 * It must have a written left neighbor.
				 * Unwrite the killed part of that one and
				 * try again.
				 */
				if (!xfs_iext_prev_extent(ifp, &icur, &prev))
					ASSERT(0);
				ASSERT(prev.br_state == XFS_EXT_NORM);
				ASSERT(!isnullstartblock(prev.br_startblock));
				ASSERT(del.br_startblock ==
				       prev.br_startblock + prev.br_blockcount);
				if (prev.br_startoff < start) {
					mod = start - prev.br_startoff;
					prev.br_blockcount -= mod;
					prev.br_startblock += mod;
					prev.br_startoff = start;
				}
				prev.br_state = XFS_EXT_UNWRITTEN;
				error = xfs_bmap_add_extent_unwritten_real(tp,
						ip, whichfork, &icur, &cur,
						&prev, firstblock, dfops,
						&logflags);
				if (error)
					goto error0;
				goto nodelete;
			} else {
				ASSERT(del.br_state == XFS_EXT_NORM);
				del.br_state = XFS_EXT_UNWRITTEN;
				error = xfs_bmap_add_extent_unwritten_real(tp,
						ip, whichfork, &icur, &cur,
						&del, firstblock, dfops,
						&logflags);
				if (error)
					goto error0;
				goto nodelete;
			}
		}

		if (wasdel) {
			error = xfs_bmap_del_extent_delay(ip, whichfork, &icur,
					&got, &del);
		} else {
			error = xfs_bmap_del_extent_real(ip, tp, &icur, dfops,
					cur, &del, &tmp_logflags, whichfork,
					flags);
			logflags |= tmp_logflags;
		}

		if (error)
			goto error0;

		max_len -= del.br_blockcount;
		end = del.br_startoff - 1;
nodelete:
		/*
		 * If not done go on to the next (previous) record.
		 */
		if (end != (xfs_fileoff_t)-1 && end >= start) {
			if (!xfs_iext_get_extent(ifp, &icur, &got) ||
			    (got.br_startoff > end &&
			     !xfs_iext_prev_extent(ifp, &icur, &got))) {
				done = true;
				break;
			}
			extno++;
		}
	}
	if (done || end == (xfs_fileoff_t)-1 || end < start)
		*rlen = 0;
	else
		*rlen = end - start + 1;

	/*
	 * Convert to a btree if necessary.
	 */
	if (xfs_bmap_needs_btree(ip, whichfork)) {
		ASSERT(cur == NULL);
		error = xfs_bmap_extents_to_btree(tp, ip, firstblock, dfops,
			&cur, 0, &tmp_logflags, whichfork);
		logflags |= tmp_logflags;
		if (error)
			goto error0;
	}
	/*
	 * transform from btree to extents, give it cur
	 */
	else if (xfs_bmap_wants_extents(ip, whichfork)) {
		ASSERT(cur != NULL);
		error = xfs_bmap_btree_to_extents(tp, ip, cur, &tmp_logflags,
			whichfork);
		logflags |= tmp_logflags;
		if (error)
			goto error0;
	}
	/*
	 * transform from extents to local?
	 */
	error = 0;
error0:
	/*
	 * Log everything.  Do this after conversion, there's no point in
	 * logging the extent records if we've converted to btree format.
	 */
	if ((logflags & xfs_ilog_fext(whichfork)) &&
	    XFS_IFORK_FORMAT(ip, whichfork) != XFS_DINODE_FMT_EXTENTS)
		logflags &= ~xfs_ilog_fext(whichfork);
	else if ((logflags & xfs_ilog_fbroot(whichfork)) &&
		 XFS_IFORK_FORMAT(ip, whichfork) != XFS_DINODE_FMT_BTREE)
		logflags &= ~xfs_ilog_fbroot(whichfork);
	/*
	 * Log inode even in the error case, if the transaction
	 * is dirty we'll need to shut down the filesystem.
	 */
	if (logflags)
		xfs_trans_log_inode(tp, ip, logflags);
	if (cur) {
		if (!error) {
			*firstblock = cur->bc_private.b.firstblock;
			cur->bc_private.b.allocated = 0;
		}
		xfs_btree_del_cursor(cur,
			error ? XFS_BTREE_ERROR : XFS_BTREE_NOERROR);
	}
	return error;
}
xfs_bmap_del_extent_real(
	xfs_inode_t		*ip,	/* incore inode pointer */
	xfs_trans_t		*tp,	/* current transaction pointer */
	struct xfs_iext_cursor	*icur,
	struct xfs_defer_ops	*dfops,	/* list of extents to be freed */
	xfs_btree_cur_t		*cur,	/* if null, not a btree */
	xfs_bmbt_irec_t		*del,	/* data to remove from extents */
	int			*logflagsp, /* inode logging flags */
	int			whichfork, /* data or attr fork */
	int			bflags)	/* bmapi flags */
{
	xfs_fsblock_t		del_endblock=0;	/* first block past del */
	xfs_fileoff_t		del_endoff;	/* first offset past del */
	int			do_fx;	/* free extent at end of routine */
	int			error;	/* error return value */
	int			flags = 0;/* inode logging flags */
	struct xfs_bmbt_irec	got;	/* current extent entry */
	xfs_fileoff_t		got_endoff;	/* first offset past got */
	int			i;	/* temp state */
	xfs_ifork_t		*ifp;	/* inode fork pointer */
	xfs_mount_t		*mp;	/* mount structure */
	xfs_filblks_t		nblks;	/* quota/sb block count */
	xfs_bmbt_irec_t		new;	/* new record to be inserted */
	/* REFERENCED */
	uint			qfield;	/* quota field to update */
	int			state = xfs_bmap_fork_to_state(whichfork);
	struct xfs_bmbt_irec	old;

	mp = ip->i_mount;
	XFS_STATS_INC(mp, xs_del_exlist);

	ifp = XFS_IFORK_PTR(ip, whichfork);
	ASSERT(del->br_blockcount > 0);
	xfs_iext_get_extent(ifp, icur, &got);
	ASSERT(got.br_startoff <= del->br_startoff);
	del_endoff = del->br_startoff + del->br_blockcount;
	got_endoff = got.br_startoff + got.br_blockcount;
	ASSERT(got_endoff >= del_endoff);
	ASSERT(!isnullstartblock(got.br_startblock));
	qfield = 0;
	error = 0;

	/*
	 * If it's the case where the directory code is running with no block
	 * reservation, and the deleted block is in the middle of its extent,
	 * and the resulting insert of an extent would cause transformation to
	 * btree format, then reject it.  The calling code will then swap blocks
	 * around instead.  We have to do this now, rather than waiting for the
	 * conversion to btree format, since the transaction will be dirty then.
	 */
	if (tp->t_blk_res == 0 &&
	    XFS_IFORK_FORMAT(ip, whichfork) == XFS_DINODE_FMT_EXTENTS &&
	    XFS_IFORK_NEXTENTS(ip, whichfork) >=
			XFS_IFORK_MAXEXT(ip, whichfork) &&
	    del->br_startoff > got.br_startoff && del_endoff < got_endoff)
		return -ENOSPC;

	flags = XFS_ILOG_CORE;
	if (whichfork == XFS_DATA_FORK && XFS_IS_REALTIME_INODE(ip)) {
		xfs_fsblock_t	bno;
		xfs_filblks_t	len;

		ASSERT(do_mod(del->br_blockcount, mp->m_sb.sb_rextsize) == 0);
		ASSERT(do_mod(del->br_startblock, mp->m_sb.sb_rextsize) == 0);
		bno = del->br_startblock;
		len = del->br_blockcount;
		do_div(bno, mp->m_sb.sb_rextsize);
		do_div(len, mp->m_sb.sb_rextsize);
		error = xfs_rtfree_extent(tp, bno, (xfs_extlen_t)len);
		if (error)
			goto done;
		do_fx = 0;
		nblks = len * mp->m_sb.sb_rextsize;
		qfield = XFS_TRANS_DQ_RTBCOUNT;
	} else {
		do_fx = 1;
		nblks = del->br_blockcount;
		qfield = XFS_TRANS_DQ_BCOUNT;
	}

	del_endblock = del->br_startblock + del->br_blockcount;
	if (cur) {
		error = xfs_bmbt_lookup_eq(cur, &got, &i);
		if (error)
			goto done;
		XFS_WANT_CORRUPTED_GOTO(mp, i == 1, done);
	}

	if (got.br_startoff == del->br_startoff)
		state |= BMAP_LEFT_FILLING;
	if (got_endoff == del_endoff)
		state |= BMAP_RIGHT_FILLING;

	switch (state & (BMAP_LEFT_FILLING | BMAP_RIGHT_FILLING)) {
	case BMAP_LEFT_FILLING | BMAP_RIGHT_FILLING:
		/*
		 * Matches the whole extent.  Delete the entry.
		 */
		xfs_iext_remove(ip, icur, state);
		xfs_iext_prev(ifp, icur);
		XFS_IFORK_NEXT_SET(ip, whichfork,
			XFS_IFORK_NEXTENTS(ip, whichfork) - 1);
		flags |= XFS_ILOG_CORE;
		if (!cur) {
			flags |= xfs_ilog_fext(whichfork);
			break;
		}
		if ((error = xfs_btree_delete(cur, &i)))
			goto done;
		XFS_WANT_CORRUPTED_GOTO(mp, i == 1, done);
		break;
	case BMAP_LEFT_FILLING:
		/*
		 * Deleting the first part of the extent.
		 */
		got.br_startoff = del_endoff;
		got.br_startblock = del_endblock;
		got.br_blockcount -= del->br_blockcount;
		xfs_iext_update_extent(ip, state, icur, &got);
		if (!cur) {
			flags |= xfs_ilog_fext(whichfork);
			break;
		}
		error = xfs_bmbt_update(cur, &got);
		if (error)
			goto done;
		break;
	case BMAP_RIGHT_FILLING:
		/*
		 * Deleting the last part of the extent.
		 */
		got.br_blockcount -= del->br_blockcount;
		xfs_iext_update_extent(ip, state, icur, &got);
		if (!cur) {
			flags |= xfs_ilog_fext(whichfork);
			break;
		}
		error = xfs_bmbt_update(cur, &got);
		if (error)
			goto done;
		break;
	case 0:
		/*
		 * Deleting the middle of the extent.
		 */
		old = got;

		got.br_blockcount = del->br_startoff - got.br_startoff;
		xfs_iext_update_extent(ip, state, icur, &got);

		new.br_startoff = del_endoff;
		new.br_blockcount = got_endoff - del_endoff;
		new.br_state = got.br_state;
		new.br_startblock = del_endblock;

		flags |= XFS_ILOG_CORE;
		if (cur) {
			error = xfs_bmbt_update(cur, &got);
			if (error)
				goto done;
			error = xfs_btree_increment(cur, 0, &i);
			if (error)
				goto done;
			cur->bc_rec.b = new;
			error = xfs_btree_insert(cur, &i);
			if (error && error != -ENOSPC)
				goto done;
			/*
			 * If get no-space back from btree insert, it tried a
			 * split, and we have a zero block reservation.  Fix up
			 * our state and return the error.
			 */
			if (error == -ENOSPC) {
				/*
				 * Reset the cursor, don't trust it after any
				 * insert operation.
				 */
				error = xfs_bmbt_lookup_eq(cur, &got, &i);
				if (error)
					goto done;
				XFS_WANT_CORRUPTED_GOTO(mp, i == 1, done);
				/*
				 * Update the btree record back
				 * to the original value.
				 */
				error = xfs_bmbt_update(cur, &old);
				if (error)
					goto done;
				/*
				 * Reset the extent record back
				 * to the original value.
				 */
				xfs_iext_update_extent(ip, state, icur, &old);
				flags = 0;
				error = -ENOSPC;
				goto done;
			}
			XFS_WANT_CORRUPTED_GOTO(mp, i == 1, done);
		} else
			flags |= xfs_ilog_fext(whichfork);
		XFS_IFORK_NEXT_SET(ip, whichfork,
			XFS_IFORK_NEXTENTS(ip, whichfork) + 1);
		xfs_iext_next(ifp, icur);
		xfs_iext_insert(ip, icur, &new, state);
		break;
	}

	/* remove reverse mapping */
	error = xfs_rmap_unmap_extent(mp, dfops, ip, whichfork, del);
	if (error)
		goto done;

	/*
	 * If we need to, add to list of extents to delete.
	 */
	if (do_fx && !(bflags & XFS_BMAPI_REMAP)) {
		if (xfs_is_reflink_inode(ip) && whichfork == XFS_DATA_FORK) {
			error = xfs_refcount_decrease_extent(mp, dfops, del);
			if (error)
				goto done;
		} else
			xfs_bmap_add_free(mp, dfops, del->br_startblock,
					del->br_blockcount, NULL);
	}

	/*
	 * Adjust inode # blocks in the file.
	 */
	if (nblks)
		ip->i_d.di_nblocks -= nblks;
	/*
	 * Adjust quota data.
	 */
	if (qfield && !(bflags & XFS_BMAPI_REMAP))
		xfs_trans_mod_dquot_byino(tp, ip, qfield, (long)-nblks);

done:
	*logflagsp = flags;
	return error;
}
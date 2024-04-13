xfs_bmap_add_extent_unwritten_real(
	struct xfs_trans	*tp,
	xfs_inode_t		*ip,	/* incore inode pointer */
	int			whichfork,
	struct xfs_iext_cursor	*icur,
	xfs_btree_cur_t		**curp,	/* if *curp is null, not a btree */
	xfs_bmbt_irec_t		*new,	/* new data to add to file extents */
	xfs_fsblock_t		*first,	/* pointer to firstblock variable */
	struct xfs_defer_ops	*dfops,	/* list of extents to be freed */
	int			*logflagsp) /* inode logging flags */
{
	xfs_btree_cur_t		*cur;	/* btree cursor */
	int			error;	/* error return value */
	int			i;	/* temp state */
	xfs_ifork_t		*ifp;	/* inode fork pointer */
	xfs_fileoff_t		new_endoff;	/* end offset of new entry */
	xfs_bmbt_irec_t		r[3];	/* neighbor extent entries */
					/* left is 0, right is 1, prev is 2 */
	int			rval=0;	/* return value (logging flags) */
	int			state = xfs_bmap_fork_to_state(whichfork);
	struct xfs_mount	*mp = ip->i_mount;
	struct xfs_bmbt_irec	old;

	*logflagsp = 0;

	cur = *curp;
	ifp = XFS_IFORK_PTR(ip, whichfork);

	ASSERT(!isnullstartblock(new->br_startblock));

	XFS_STATS_INC(mp, xs_add_exlist);

#define	LEFT		r[0]
#define	RIGHT		r[1]
#define	PREV		r[2]

	/*
	 * Set up a bunch of variables to make the tests simpler.
	 */
	error = 0;
	xfs_iext_get_extent(ifp, icur, &PREV);
	ASSERT(new->br_state != PREV.br_state);
	new_endoff = new->br_startoff + new->br_blockcount;
	ASSERT(PREV.br_startoff <= new->br_startoff);
	ASSERT(PREV.br_startoff + PREV.br_blockcount >= new_endoff);

	/*
	 * Set flags determining what part of the previous oldext allocation
	 * extent is being replaced by a newext allocation.
	 */
	if (PREV.br_startoff == new->br_startoff)
		state |= BMAP_LEFT_FILLING;
	if (PREV.br_startoff + PREV.br_blockcount == new_endoff)
		state |= BMAP_RIGHT_FILLING;

	/*
	 * Check and set flags if this segment has a left neighbor.
	 * Don't set contiguous if the combined extent would be too large.
	 */
	if (xfs_iext_peek_prev_extent(ifp, icur, &LEFT)) {
		state |= BMAP_LEFT_VALID;
		if (isnullstartblock(LEFT.br_startblock))
			state |= BMAP_LEFT_DELAY;
	}

	if ((state & BMAP_LEFT_VALID) && !(state & BMAP_LEFT_DELAY) &&
	    LEFT.br_startoff + LEFT.br_blockcount == new->br_startoff &&
	    LEFT.br_startblock + LEFT.br_blockcount == new->br_startblock &&
	    LEFT.br_state == new->br_state &&
	    LEFT.br_blockcount + new->br_blockcount <= MAXEXTLEN)
		state |= BMAP_LEFT_CONTIG;

	/*
	 * Check and set flags if this segment has a right neighbor.
	 * Don't set contiguous if the combined extent would be too large.
	 * Also check for all-three-contiguous being too large.
	 */
	if (xfs_iext_peek_next_extent(ifp, icur, &RIGHT)) {
		state |= BMAP_RIGHT_VALID;
		if (isnullstartblock(RIGHT.br_startblock))
			state |= BMAP_RIGHT_DELAY;
	}

	if ((state & BMAP_RIGHT_VALID) && !(state & BMAP_RIGHT_DELAY) &&
	    new_endoff == RIGHT.br_startoff &&
	    new->br_startblock + new->br_blockcount == RIGHT.br_startblock &&
	    new->br_state == RIGHT.br_state &&
	    new->br_blockcount + RIGHT.br_blockcount <= MAXEXTLEN &&
	    ((state & (BMAP_LEFT_CONTIG | BMAP_LEFT_FILLING |
		       BMAP_RIGHT_FILLING)) !=
		      (BMAP_LEFT_CONTIG | BMAP_LEFT_FILLING |
		       BMAP_RIGHT_FILLING) ||
	     LEFT.br_blockcount + new->br_blockcount + RIGHT.br_blockcount
			<= MAXEXTLEN))
		state |= BMAP_RIGHT_CONTIG;

	/*
	 * Switch out based on the FILLING and CONTIG state bits.
	 */
	switch (state & (BMAP_LEFT_FILLING | BMAP_LEFT_CONTIG |
			 BMAP_RIGHT_FILLING | BMAP_RIGHT_CONTIG)) {
	case BMAP_LEFT_FILLING | BMAP_LEFT_CONTIG |
	     BMAP_RIGHT_FILLING | BMAP_RIGHT_CONTIG:
		/*
		 * Setting all of a previous oldext extent to newext.
		 * The left and right neighbors are both contiguous with new.
		 */
		LEFT.br_blockcount += PREV.br_blockcount + RIGHT.br_blockcount;

		xfs_iext_remove(ip, icur, state);
		xfs_iext_remove(ip, icur, state);
		xfs_iext_prev(ifp, icur);
		xfs_iext_update_extent(ip, state, icur, &LEFT);
		XFS_IFORK_NEXT_SET(ip, whichfork,
				XFS_IFORK_NEXTENTS(ip, whichfork) - 2);
		if (cur == NULL)
			rval = XFS_ILOG_CORE | XFS_ILOG_DEXT;
		else {
			rval = XFS_ILOG_CORE;
			error = xfs_bmbt_lookup_eq(cur, &RIGHT, &i);
			if (error)
				goto done;
			XFS_WANT_CORRUPTED_GOTO(mp, i == 1, done);
			if ((error = xfs_btree_delete(cur, &i)))
				goto done;
			XFS_WANT_CORRUPTED_GOTO(mp, i == 1, done);
			if ((error = xfs_btree_decrement(cur, 0, &i)))
				goto done;
			XFS_WANT_CORRUPTED_GOTO(mp, i == 1, done);
			if ((error = xfs_btree_delete(cur, &i)))
				goto done;
			XFS_WANT_CORRUPTED_GOTO(mp, i == 1, done);
			if ((error = xfs_btree_decrement(cur, 0, &i)))
				goto done;
			XFS_WANT_CORRUPTED_GOTO(mp, i == 1, done);
			error = xfs_bmbt_update(cur, &LEFT);
			if (error)
				goto done;
		}
		break;

	case BMAP_LEFT_FILLING | BMAP_RIGHT_FILLING | BMAP_LEFT_CONTIG:
		/*
		 * Setting all of a previous oldext extent to newext.
		 * The left neighbor is contiguous, the right is not.
		 */
		LEFT.br_blockcount += PREV.br_blockcount;

		xfs_iext_remove(ip, icur, state);
		xfs_iext_prev(ifp, icur);
		xfs_iext_update_extent(ip, state, icur, &LEFT);
		XFS_IFORK_NEXT_SET(ip, whichfork,
				XFS_IFORK_NEXTENTS(ip, whichfork) - 1);
		if (cur == NULL)
			rval = XFS_ILOG_CORE | XFS_ILOG_DEXT;
		else {
			rval = XFS_ILOG_CORE;
			error = xfs_bmbt_lookup_eq(cur, &PREV, &i);
			if (error)
				goto done;
			XFS_WANT_CORRUPTED_GOTO(mp, i == 1, done);
			if ((error = xfs_btree_delete(cur, &i)))
				goto done;
			XFS_WANT_CORRUPTED_GOTO(mp, i == 1, done);
			if ((error = xfs_btree_decrement(cur, 0, &i)))
				goto done;
			XFS_WANT_CORRUPTED_GOTO(mp, i == 1, done);
			error = xfs_bmbt_update(cur, &LEFT);
			if (error)
				goto done;
		}
		break;

	case BMAP_LEFT_FILLING | BMAP_RIGHT_FILLING | BMAP_RIGHT_CONTIG:
		/*
		 * Setting all of a previous oldext extent to newext.
		 * The right neighbor is contiguous, the left is not.
		 */
		PREV.br_blockcount += RIGHT.br_blockcount;
		PREV.br_state = new->br_state;

		xfs_iext_next(ifp, icur);
		xfs_iext_remove(ip, icur, state);
		xfs_iext_prev(ifp, icur);
		xfs_iext_update_extent(ip, state, icur, &PREV);

		XFS_IFORK_NEXT_SET(ip, whichfork,
				XFS_IFORK_NEXTENTS(ip, whichfork) - 1);
		if (cur == NULL)
			rval = XFS_ILOG_CORE | XFS_ILOG_DEXT;
		else {
			rval = XFS_ILOG_CORE;
			error = xfs_bmbt_lookup_eq(cur, &RIGHT, &i);
			if (error)
				goto done;
			XFS_WANT_CORRUPTED_GOTO(mp, i == 1, done);
			if ((error = xfs_btree_delete(cur, &i)))
				goto done;
			XFS_WANT_CORRUPTED_GOTO(mp, i == 1, done);
			if ((error = xfs_btree_decrement(cur, 0, &i)))
				goto done;
			XFS_WANT_CORRUPTED_GOTO(mp, i == 1, done);
			error = xfs_bmbt_update(cur, &PREV);
			if (error)
				goto done;
		}
		break;

	case BMAP_LEFT_FILLING | BMAP_RIGHT_FILLING:
		/*
		 * Setting all of a previous oldext extent to newext.
		 * Neither the left nor right neighbors are contiguous with
		 * the new one.
		 */
		PREV.br_state = new->br_state;
		xfs_iext_update_extent(ip, state, icur, &PREV);

		if (cur == NULL)
			rval = XFS_ILOG_DEXT;
		else {
			rval = 0;
			error = xfs_bmbt_lookup_eq(cur, new, &i);
			if (error)
				goto done;
			XFS_WANT_CORRUPTED_GOTO(mp, i == 1, done);
			error = xfs_bmbt_update(cur, &PREV);
			if (error)
				goto done;
		}
		break;

	case BMAP_LEFT_FILLING | BMAP_LEFT_CONTIG:
		/*
		 * Setting the first part of a previous oldext extent to newext.
		 * The left neighbor is contiguous.
		 */
		LEFT.br_blockcount += new->br_blockcount;

		old = PREV;
		PREV.br_startoff += new->br_blockcount;
		PREV.br_startblock += new->br_blockcount;
		PREV.br_blockcount -= new->br_blockcount;

		xfs_iext_update_extent(ip, state, icur, &PREV);
		xfs_iext_prev(ifp, icur);
		xfs_iext_update_extent(ip, state, icur, &LEFT);

		if (cur == NULL)
			rval = XFS_ILOG_DEXT;
		else {
			rval = 0;
			error = xfs_bmbt_lookup_eq(cur, &old, &i);
			if (error)
				goto done;
			XFS_WANT_CORRUPTED_GOTO(mp, i == 1, done);
			error = xfs_bmbt_update(cur, &PREV);
			if (error)
				goto done;
			error = xfs_btree_decrement(cur, 0, &i);
			if (error)
				goto done;
			error = xfs_bmbt_update(cur, &LEFT);
			if (error)
				goto done;
		}
		break;

	case BMAP_LEFT_FILLING:
		/*
		 * Setting the first part of a previous oldext extent to newext.
		 * The left neighbor is not contiguous.
		 */
		old = PREV;
		PREV.br_startoff += new->br_blockcount;
		PREV.br_startblock += new->br_blockcount;
		PREV.br_blockcount -= new->br_blockcount;

		xfs_iext_update_extent(ip, state, icur, &PREV);
		xfs_iext_insert(ip, icur, new, state);
		XFS_IFORK_NEXT_SET(ip, whichfork,
				XFS_IFORK_NEXTENTS(ip, whichfork) + 1);
		if (cur == NULL)
			rval = XFS_ILOG_CORE | XFS_ILOG_DEXT;
		else {
			rval = XFS_ILOG_CORE;
			error = xfs_bmbt_lookup_eq(cur, &old, &i);
			if (error)
				goto done;
			XFS_WANT_CORRUPTED_GOTO(mp, i == 1, done);
			error = xfs_bmbt_update(cur, &PREV);
			if (error)
				goto done;
			cur->bc_rec.b = *new;
			if ((error = xfs_btree_insert(cur, &i)))
				goto done;
			XFS_WANT_CORRUPTED_GOTO(mp, i == 1, done);
		}
		break;

	case BMAP_RIGHT_FILLING | BMAP_RIGHT_CONTIG:
		/*
		 * Setting the last part of a previous oldext extent to newext.
		 * The right neighbor is contiguous with the new allocation.
		 */
		old = PREV;
		PREV.br_blockcount -= new->br_blockcount;

		RIGHT.br_startoff = new->br_startoff;
		RIGHT.br_startblock = new->br_startblock;
		RIGHT.br_blockcount += new->br_blockcount;

		xfs_iext_update_extent(ip, state, icur, &PREV);
		xfs_iext_next(ifp, icur);
		xfs_iext_update_extent(ip, state, icur, &RIGHT);

		if (cur == NULL)
			rval = XFS_ILOG_DEXT;
		else {
			rval = 0;
			error = xfs_bmbt_lookup_eq(cur, &old, &i);
			if (error)
				goto done;
			XFS_WANT_CORRUPTED_GOTO(mp, i == 1, done);
			error = xfs_bmbt_update(cur, &PREV);
			if (error)
				goto done;
			error = xfs_btree_increment(cur, 0, &i);
			if (error)
				goto done;
			error = xfs_bmbt_update(cur, &RIGHT);
			if (error)
				goto done;
		}
		break;

	case BMAP_RIGHT_FILLING:
		/*
		 * Setting the last part of a previous oldext extent to newext.
		 * The right neighbor is not contiguous.
		 */
		old = PREV;
		PREV.br_blockcount -= new->br_blockcount;

		xfs_iext_update_extent(ip, state, icur, &PREV);
		xfs_iext_next(ifp, icur);
		xfs_iext_insert(ip, icur, new, state);

		XFS_IFORK_NEXT_SET(ip, whichfork,
				XFS_IFORK_NEXTENTS(ip, whichfork) + 1);
		if (cur == NULL)
			rval = XFS_ILOG_CORE | XFS_ILOG_DEXT;
		else {
			rval = XFS_ILOG_CORE;
			error = xfs_bmbt_lookup_eq(cur, &old, &i);
			if (error)
				goto done;
			XFS_WANT_CORRUPTED_GOTO(mp, i == 1, done);
			error = xfs_bmbt_update(cur, &PREV);
			if (error)
				goto done;
			error = xfs_bmbt_lookup_eq(cur, new, &i);
			if (error)
				goto done;
			XFS_WANT_CORRUPTED_GOTO(mp, i == 0, done);
			if ((error = xfs_btree_insert(cur, &i)))
				goto done;
			XFS_WANT_CORRUPTED_GOTO(mp, i == 1, done);
		}
		break;

	case 0:
		/*
		 * Setting the middle part of a previous oldext extent to
		 * newext.  Contiguity is impossible here.
		 * One extent becomes three extents.
		 */
		old = PREV;
		PREV.br_blockcount = new->br_startoff - PREV.br_startoff;

		r[0] = *new;
		r[1].br_startoff = new_endoff;
		r[1].br_blockcount =
			old.br_startoff + old.br_blockcount - new_endoff;
		r[1].br_startblock = new->br_startblock + new->br_blockcount;
		r[1].br_state = PREV.br_state;

		xfs_iext_update_extent(ip, state, icur, &PREV);
		xfs_iext_next(ifp, icur);
		xfs_iext_insert(ip, icur, &r[1], state);
		xfs_iext_insert(ip, icur, &r[0], state);

		XFS_IFORK_NEXT_SET(ip, whichfork,
				XFS_IFORK_NEXTENTS(ip, whichfork) + 2);
		if (cur == NULL)
			rval = XFS_ILOG_CORE | XFS_ILOG_DEXT;
		else {
			rval = XFS_ILOG_CORE;
			error = xfs_bmbt_lookup_eq(cur, &old, &i);
			if (error)
				goto done;
			XFS_WANT_CORRUPTED_GOTO(mp, i == 1, done);
			/* new right extent - oldext */
			error = xfs_bmbt_update(cur, &r[1]);
			if (error)
				goto done;
			/* new left extent - oldext */
			cur->bc_rec.b = PREV;
			if ((error = xfs_btree_insert(cur, &i)))
				goto done;
			XFS_WANT_CORRUPTED_GOTO(mp, i == 1, done);
			/*
			 * Reset the cursor to the position of the new extent
			 * we are about to insert as we can't trust it after
			 * the previous insert.
			 */
			error = xfs_bmbt_lookup_eq(cur, new, &i);
			if (error)
				goto done;
			XFS_WANT_CORRUPTED_GOTO(mp, i == 0, done);
			/* new middle extent - newext */
			if ((error = xfs_btree_insert(cur, &i)))
				goto done;
			XFS_WANT_CORRUPTED_GOTO(mp, i == 1, done);
		}
		break;

	case BMAP_LEFT_FILLING | BMAP_LEFT_CONTIG | BMAP_RIGHT_CONTIG:
	case BMAP_RIGHT_FILLING | BMAP_LEFT_CONTIG | BMAP_RIGHT_CONTIG:
	case BMAP_LEFT_FILLING | BMAP_RIGHT_CONTIG:
	case BMAP_RIGHT_FILLING | BMAP_LEFT_CONTIG:
	case BMAP_LEFT_CONTIG | BMAP_RIGHT_CONTIG:
	case BMAP_LEFT_CONTIG:
	case BMAP_RIGHT_CONTIG:
		/*
		 * These cases are all impossible.
		 */
		ASSERT(0);
	}

	/* update reverse mappings */
	error = xfs_rmap_convert_extent(mp, dfops, ip, whichfork, new);
	if (error)
		goto done;

	/* convert to a btree if necessary */
	if (xfs_bmap_needs_btree(ip, whichfork)) {
		int	tmp_logflags;	/* partial log flag return val */

		ASSERT(cur == NULL);
		error = xfs_bmap_extents_to_btree(tp, ip, first, dfops, &cur,
				0, &tmp_logflags, whichfork);
		*logflagsp |= tmp_logflags;
		if (error)
			goto done;
	}

	/* clear out the allocated field, done with it now in any case. */
	if (cur) {
		cur->bc_private.b.allocated = 0;
		*curp = cur;
	}

	xfs_bmap_check_leaf_extents(*curp, ip, whichfork);
done:
	*logflagsp |= rval;
	return error;
#undef	LEFT
#undef	RIGHT
#undef	PREV
}
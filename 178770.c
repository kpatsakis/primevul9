xfs_bmap_add_extent_delay_real(
	struct xfs_bmalloca	*bma,
	int			whichfork)
{
	struct xfs_bmbt_irec	*new = &bma->got;
	int			error;	/* error return value */
	int			i;	/* temp state */
	xfs_ifork_t		*ifp;	/* inode fork pointer */
	xfs_fileoff_t		new_endoff;	/* end offset of new entry */
	xfs_bmbt_irec_t		r[3];	/* neighbor extent entries */
					/* left is 0, right is 1, prev is 2 */
	int			rval=0;	/* return value (logging flags) */
	int			state = xfs_bmap_fork_to_state(whichfork);
	xfs_filblks_t		da_new; /* new count del alloc blocks used */
	xfs_filblks_t		da_old; /* old count del alloc blocks used */
	xfs_filblks_t		temp=0;	/* value for da_new calculations */
	int			tmp_rval;	/* partial logging flags */
	struct xfs_mount	*mp;
	xfs_extnum_t		*nextents;
	struct xfs_bmbt_irec	old;

	mp = bma->ip->i_mount;
	ifp = XFS_IFORK_PTR(bma->ip, whichfork);
	ASSERT(whichfork != XFS_ATTR_FORK);
	nextents = (whichfork == XFS_COW_FORK ? &bma->ip->i_cnextents :
						&bma->ip->i_d.di_nextents);

	ASSERT(!isnullstartblock(new->br_startblock));
	ASSERT(!bma->cur ||
	       (bma->cur->bc_private.b.flags & XFS_BTCUR_BPRV_WASDEL));

	XFS_STATS_INC(mp, xs_add_exlist);

#define	LEFT		r[0]
#define	RIGHT		r[1]
#define	PREV		r[2]

	/*
	 * Set up a bunch of variables to make the tests simpler.
	 */
	xfs_iext_get_extent(ifp, &bma->icur, &PREV);
	new_endoff = new->br_startoff + new->br_blockcount;
	ASSERT(isnullstartblock(PREV.br_startblock));
	ASSERT(PREV.br_startoff <= new->br_startoff);
	ASSERT(PREV.br_startoff + PREV.br_blockcount >= new_endoff);

	da_old = startblockval(PREV.br_startblock);
	da_new = 0;

	/*
	 * Set flags determining what part of the previous delayed allocation
	 * extent is being replaced by a real allocation.
	 */
	if (PREV.br_startoff == new->br_startoff)
		state |= BMAP_LEFT_FILLING;
	if (PREV.br_startoff + PREV.br_blockcount == new_endoff)
		state |= BMAP_RIGHT_FILLING;

	/*
	 * Check and set flags if this segment has a left neighbor.
	 * Don't set contiguous if the combined extent would be too large.
	 */
	if (xfs_iext_peek_prev_extent(ifp, &bma->icur, &LEFT)) {
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
	if (xfs_iext_peek_next_extent(ifp, &bma->icur, &RIGHT)) {
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

	error = 0;
	/*
	 * Switch out based on the FILLING and CONTIG state bits.
	 */
	switch (state & (BMAP_LEFT_FILLING | BMAP_LEFT_CONTIG |
			 BMAP_RIGHT_FILLING | BMAP_RIGHT_CONTIG)) {
	case BMAP_LEFT_FILLING | BMAP_LEFT_CONTIG |
	     BMAP_RIGHT_FILLING | BMAP_RIGHT_CONTIG:
		/*
		 * Filling in all of a previously delayed allocation extent.
		 * The left and right neighbors are both contiguous with new.
		 */
		LEFT.br_blockcount += PREV.br_blockcount + RIGHT.br_blockcount;

		xfs_iext_remove(bma->ip, &bma->icur, state);
		xfs_iext_remove(bma->ip, &bma->icur, state);
		xfs_iext_prev(ifp, &bma->icur);
		xfs_iext_update_extent(bma->ip, state, &bma->icur, &LEFT);
		(*nextents)--;

		if (bma->cur == NULL)
			rval = XFS_ILOG_CORE | XFS_ILOG_DEXT;
		else {
			rval = XFS_ILOG_CORE;
			error = xfs_bmbt_lookup_eq(bma->cur, &RIGHT, &i);
			if (error)
				goto done;
			XFS_WANT_CORRUPTED_GOTO(mp, i == 1, done);
			error = xfs_btree_delete(bma->cur, &i);
			if (error)
				goto done;
			XFS_WANT_CORRUPTED_GOTO(mp, i == 1, done);
			error = xfs_btree_decrement(bma->cur, 0, &i);
			if (error)
				goto done;
			XFS_WANT_CORRUPTED_GOTO(mp, i == 1, done);
			error = xfs_bmbt_update(bma->cur, &LEFT);
			if (error)
				goto done;
		}
		break;

	case BMAP_LEFT_FILLING | BMAP_RIGHT_FILLING | BMAP_LEFT_CONTIG:
		/*
		 * Filling in all of a previously delayed allocation extent.
		 * The left neighbor is contiguous, the right is not.
		 */
		old = LEFT;
		LEFT.br_blockcount += PREV.br_blockcount;

		xfs_iext_remove(bma->ip, &bma->icur, state);
		xfs_iext_prev(ifp, &bma->icur);
		xfs_iext_update_extent(bma->ip, state, &bma->icur, &LEFT);

		if (bma->cur == NULL)
			rval = XFS_ILOG_DEXT;
		else {
			rval = 0;
			error = xfs_bmbt_lookup_eq(bma->cur, &old, &i);
			if (error)
				goto done;
			XFS_WANT_CORRUPTED_GOTO(mp, i == 1, done);
			error = xfs_bmbt_update(bma->cur, &LEFT);
			if (error)
				goto done;
		}
		break;

	case BMAP_LEFT_FILLING | BMAP_RIGHT_FILLING | BMAP_RIGHT_CONTIG:
		/*
		 * Filling in all of a previously delayed allocation extent.
		 * The right neighbor is contiguous, the left is not.
		 */
		PREV.br_startblock = new->br_startblock;
		PREV.br_blockcount += RIGHT.br_blockcount;

		xfs_iext_next(ifp, &bma->icur);
		xfs_iext_remove(bma->ip, &bma->icur, state);
		xfs_iext_prev(ifp, &bma->icur);
		xfs_iext_update_extent(bma->ip, state, &bma->icur, &PREV);

		if (bma->cur == NULL)
			rval = XFS_ILOG_DEXT;
		else {
			rval = 0;
			error = xfs_bmbt_lookup_eq(bma->cur, &RIGHT, &i);
			if (error)
				goto done;
			XFS_WANT_CORRUPTED_GOTO(mp, i == 1, done);
			error = xfs_bmbt_update(bma->cur, &PREV);
			if (error)
				goto done;
		}
		break;

	case BMAP_LEFT_FILLING | BMAP_RIGHT_FILLING:
		/*
		 * Filling in all of a previously delayed allocation extent.
		 * Neither the left nor right neighbors are contiguous with
		 * the new one.
		 */
		PREV.br_startblock = new->br_startblock;
		PREV.br_state = new->br_state;
		xfs_iext_update_extent(bma->ip, state, &bma->icur, &PREV);

		(*nextents)++;
		if (bma->cur == NULL)
			rval = XFS_ILOG_CORE | XFS_ILOG_DEXT;
		else {
			rval = XFS_ILOG_CORE;
			error = xfs_bmbt_lookup_eq(bma->cur, new, &i);
			if (error)
				goto done;
			XFS_WANT_CORRUPTED_GOTO(mp, i == 0, done);
			error = xfs_btree_insert(bma->cur, &i);
			if (error)
				goto done;
			XFS_WANT_CORRUPTED_GOTO(mp, i == 1, done);
		}
		break;

	case BMAP_LEFT_FILLING | BMAP_LEFT_CONTIG:
		/*
		 * Filling in the first part of a previous delayed allocation.
		 * The left neighbor is contiguous.
		 */
		old = LEFT;
		temp = PREV.br_blockcount - new->br_blockcount;
		da_new = XFS_FILBLKS_MIN(xfs_bmap_worst_indlen(bma->ip, temp),
				startblockval(PREV.br_startblock));

		LEFT.br_blockcount += new->br_blockcount;

		PREV.br_blockcount = temp;
		PREV.br_startoff += new->br_blockcount;
		PREV.br_startblock = nullstartblock(da_new);

		xfs_iext_update_extent(bma->ip, state, &bma->icur, &PREV);
		xfs_iext_prev(ifp, &bma->icur);
		xfs_iext_update_extent(bma->ip, state, &bma->icur, &LEFT);

		if (bma->cur == NULL)
			rval = XFS_ILOG_DEXT;
		else {
			rval = 0;
			error = xfs_bmbt_lookup_eq(bma->cur, &old, &i);
			if (error)
				goto done;
			XFS_WANT_CORRUPTED_GOTO(mp, i == 1, done);
			error = xfs_bmbt_update(bma->cur, &LEFT);
			if (error)
				goto done;
		}
		break;

	case BMAP_LEFT_FILLING:
		/*
		 * Filling in the first part of a previous delayed allocation.
		 * The left neighbor is not contiguous.
		 */
		xfs_iext_update_extent(bma->ip, state, &bma->icur, new);
		(*nextents)++;
		if (bma->cur == NULL)
			rval = XFS_ILOG_CORE | XFS_ILOG_DEXT;
		else {
			rval = XFS_ILOG_CORE;
			error = xfs_bmbt_lookup_eq(bma->cur, new, &i);
			if (error)
				goto done;
			XFS_WANT_CORRUPTED_GOTO(mp, i == 0, done);
			error = xfs_btree_insert(bma->cur, &i);
			if (error)
				goto done;
			XFS_WANT_CORRUPTED_GOTO(mp, i == 1, done);
		}

		if (xfs_bmap_needs_btree(bma->ip, whichfork)) {
			error = xfs_bmap_extents_to_btree(bma->tp, bma->ip,
					bma->firstblock, bma->dfops,
					&bma->cur, 1, &tmp_rval, whichfork);
			rval |= tmp_rval;
			if (error)
				goto done;
		}

		temp = PREV.br_blockcount - new->br_blockcount;
		da_new = XFS_FILBLKS_MIN(xfs_bmap_worst_indlen(bma->ip, temp),
			startblockval(PREV.br_startblock) -
			(bma->cur ? bma->cur->bc_private.b.allocated : 0));

		PREV.br_startoff = new_endoff;
		PREV.br_blockcount = temp;
		PREV.br_startblock = nullstartblock(da_new);
		xfs_iext_next(ifp, &bma->icur);
		xfs_iext_insert(bma->ip, &bma->icur, &PREV, state);
		xfs_iext_prev(ifp, &bma->icur);
		break;

	case BMAP_RIGHT_FILLING | BMAP_RIGHT_CONTIG:
		/*
		 * Filling in the last part of a previous delayed allocation.
		 * The right neighbor is contiguous with the new allocation.
		 */
		old = RIGHT;
		RIGHT.br_startoff = new->br_startoff;
		RIGHT.br_startblock = new->br_startblock;
		RIGHT.br_blockcount += new->br_blockcount;

		if (bma->cur == NULL)
			rval = XFS_ILOG_DEXT;
		else {
			rval = 0;
			error = xfs_bmbt_lookup_eq(bma->cur, &old, &i);
			if (error)
				goto done;
			XFS_WANT_CORRUPTED_GOTO(mp, i == 1, done);
			error = xfs_bmbt_update(bma->cur, &RIGHT);
			if (error)
				goto done;
		}

		temp = PREV.br_blockcount - new->br_blockcount;
		da_new = XFS_FILBLKS_MIN(xfs_bmap_worst_indlen(bma->ip, temp),
			startblockval(PREV.br_startblock));

		PREV.br_blockcount = temp;
		PREV.br_startblock = nullstartblock(da_new);

		xfs_iext_update_extent(bma->ip, state, &bma->icur, &PREV);
		xfs_iext_next(ifp, &bma->icur);
		xfs_iext_update_extent(bma->ip, state, &bma->icur, &RIGHT);
		break;

	case BMAP_RIGHT_FILLING:
		/*
		 * Filling in the last part of a previous delayed allocation.
		 * The right neighbor is not contiguous.
		 */
		xfs_iext_update_extent(bma->ip, state, &bma->icur, new);
		(*nextents)++;
		if (bma->cur == NULL)
			rval = XFS_ILOG_CORE | XFS_ILOG_DEXT;
		else {
			rval = XFS_ILOG_CORE;
			error = xfs_bmbt_lookup_eq(bma->cur, new, &i);
			if (error)
				goto done;
			XFS_WANT_CORRUPTED_GOTO(mp, i == 0, done);
			error = xfs_btree_insert(bma->cur, &i);
			if (error)
				goto done;
			XFS_WANT_CORRUPTED_GOTO(mp, i == 1, done);
		}

		if (xfs_bmap_needs_btree(bma->ip, whichfork)) {
			error = xfs_bmap_extents_to_btree(bma->tp, bma->ip,
				bma->firstblock, bma->dfops, &bma->cur, 1,
				&tmp_rval, whichfork);
			rval |= tmp_rval;
			if (error)
				goto done;
		}

		temp = PREV.br_blockcount - new->br_blockcount;
		da_new = XFS_FILBLKS_MIN(xfs_bmap_worst_indlen(bma->ip, temp),
			startblockval(PREV.br_startblock) -
			(bma->cur ? bma->cur->bc_private.b.allocated : 0));

		PREV.br_startblock = nullstartblock(da_new);
		PREV.br_blockcount = temp;
		xfs_iext_insert(bma->ip, &bma->icur, &PREV, state);
		xfs_iext_next(ifp, &bma->icur);
		break;

	case 0:
		/*
		 * Filling in the middle part of a previous delayed allocation.
		 * Contiguity is impossible here.
		 * This case is avoided almost all the time.
		 *
		 * We start with a delayed allocation:
		 *
		 * +ddddddddddddddddddddddddddddddddddddddddddddddddddddddd+
		 *  PREV @ idx
		 *
	         * and we are allocating:
		 *                     +rrrrrrrrrrrrrrrrr+
		 *			      new
		 *
		 * and we set it up for insertion as:
		 * +ddddddddddddddddddd+rrrrrrrrrrrrrrrrr+ddddddddddddddddd+
		 *                            new
		 *  PREV @ idx          LEFT              RIGHT
		 *                      inserted at idx + 1
		 */
		old = PREV;

		/* LEFT is the new middle */
		LEFT = *new;

		/* RIGHT is the new right */
		RIGHT.br_state = PREV.br_state;
		RIGHT.br_startoff = new_endoff;
		RIGHT.br_blockcount =
			PREV.br_startoff + PREV.br_blockcount - new_endoff;
		RIGHT.br_startblock =
			nullstartblock(xfs_bmap_worst_indlen(bma->ip,
					RIGHT.br_blockcount));

		/* truncate PREV */
		PREV.br_blockcount = new->br_startoff - PREV.br_startoff;
		PREV.br_startblock =
			nullstartblock(xfs_bmap_worst_indlen(bma->ip,
					PREV.br_blockcount));
		xfs_iext_update_extent(bma->ip, state, &bma->icur, &PREV);

		xfs_iext_next(ifp, &bma->icur);
		xfs_iext_insert(bma->ip, &bma->icur, &RIGHT, state);
		xfs_iext_insert(bma->ip, &bma->icur, &LEFT, state);
		(*nextents)++;

		if (bma->cur == NULL)
			rval = XFS_ILOG_CORE | XFS_ILOG_DEXT;
		else {
			rval = XFS_ILOG_CORE;
			error = xfs_bmbt_lookup_eq(bma->cur, new, &i);
			if (error)
				goto done;
			XFS_WANT_CORRUPTED_GOTO(mp, i == 0, done);
			error = xfs_btree_insert(bma->cur, &i);
			if (error)
				goto done;
			XFS_WANT_CORRUPTED_GOTO(mp, i == 1, done);
		}

		if (xfs_bmap_needs_btree(bma->ip, whichfork)) {
			error = xfs_bmap_extents_to_btree(bma->tp, bma->ip,
					bma->firstblock, bma->dfops, &bma->cur,
					1, &tmp_rval, whichfork);
			rval |= tmp_rval;
			if (error)
				goto done;
		}

		da_new = startblockval(PREV.br_startblock) +
			 startblockval(RIGHT.br_startblock);
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

	/* add reverse mapping */
	error = xfs_rmap_map_extent(mp, bma->dfops, bma->ip, whichfork, new);
	if (error)
		goto done;

	/* convert to a btree if necessary */
	if (xfs_bmap_needs_btree(bma->ip, whichfork)) {
		int	tmp_logflags;	/* partial log flag return val */

		ASSERT(bma->cur == NULL);
		error = xfs_bmap_extents_to_btree(bma->tp, bma->ip,
				bma->firstblock, bma->dfops, &bma->cur,
				da_old > 0, &tmp_logflags, whichfork);
		bma->logflags |= tmp_logflags;
		if (error)
			goto done;
	}

	if (bma->cur) {
		da_new += bma->cur->bc_private.b.allocated;
		bma->cur->bc_private.b.allocated = 0;
	}

	/* adjust for changes in reserved delayed indirect blocks */
	if (da_new != da_old) {
		ASSERT(state == 0 || da_new < da_old);
		error = xfs_mod_fdblocks(mp, (int64_t)(da_old - da_new),
				false);
	}

	xfs_bmap_check_leaf_extents(bma->cur, bma->ip, whichfork);
done:
	if (whichfork != XFS_COW_FORK)
		bma->logflags |= rval;
	return error;
#undef	LEFT
#undef	RIGHT
#undef	PREV
}
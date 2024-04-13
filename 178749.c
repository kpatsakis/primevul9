xfs_bmap_add_extent_hole_real(
	struct xfs_trans	*tp,
	struct xfs_inode	*ip,
	int			whichfork,
	struct xfs_iext_cursor	*icur,
	struct xfs_btree_cur	**curp,
	struct xfs_bmbt_irec	*new,
	xfs_fsblock_t		*first,
	struct xfs_defer_ops	*dfops,
	int			*logflagsp)
{
	struct xfs_ifork	*ifp = XFS_IFORK_PTR(ip, whichfork);
	struct xfs_mount	*mp = ip->i_mount;
	struct xfs_btree_cur	*cur = *curp;
	int			error;	/* error return value */
	int			i;	/* temp state */
	xfs_bmbt_irec_t		left;	/* left neighbor extent entry */
	xfs_bmbt_irec_t		right;	/* right neighbor extent entry */
	int			rval=0;	/* return value (logging flags) */
	int			state = xfs_bmap_fork_to_state(whichfork);
	struct xfs_bmbt_irec	old;

	ASSERT(!isnullstartblock(new->br_startblock));
	ASSERT(!cur || !(cur->bc_private.b.flags & XFS_BTCUR_BPRV_WASDEL));

	XFS_STATS_INC(mp, xs_add_exlist);

	/*
	 * Check and set flags if this segment has a left neighbor.
	 */
	if (xfs_iext_peek_prev_extent(ifp, icur, &left)) {
		state |= BMAP_LEFT_VALID;
		if (isnullstartblock(left.br_startblock))
			state |= BMAP_LEFT_DELAY;
	}

	/*
	 * Check and set flags if this segment has a current value.
	 * Not true if we're inserting into the "hole" at eof.
	 */
	if (xfs_iext_get_extent(ifp, icur, &right)) {
		state |= BMAP_RIGHT_VALID;
		if (isnullstartblock(right.br_startblock))
			state |= BMAP_RIGHT_DELAY;
	}

	/*
	 * We're inserting a real allocation between "left" and "right".
	 * Set the contiguity flags.  Don't let extents get too large.
	 */
	if ((state & BMAP_LEFT_VALID) && !(state & BMAP_LEFT_DELAY) &&
	    left.br_startoff + left.br_blockcount == new->br_startoff &&
	    left.br_startblock + left.br_blockcount == new->br_startblock &&
	    left.br_state == new->br_state &&
	    left.br_blockcount + new->br_blockcount <= MAXEXTLEN)
		state |= BMAP_LEFT_CONTIG;

	if ((state & BMAP_RIGHT_VALID) && !(state & BMAP_RIGHT_DELAY) &&
	    new->br_startoff + new->br_blockcount == right.br_startoff &&
	    new->br_startblock + new->br_blockcount == right.br_startblock &&
	    new->br_state == right.br_state &&
	    new->br_blockcount + right.br_blockcount <= MAXEXTLEN &&
	    (!(state & BMAP_LEFT_CONTIG) ||
	     left.br_blockcount + new->br_blockcount +
	     right.br_blockcount <= MAXEXTLEN))
		state |= BMAP_RIGHT_CONTIG;

	error = 0;
	/*
	 * Select which case we're in here, and implement it.
	 */
	switch (state & (BMAP_LEFT_CONTIG | BMAP_RIGHT_CONTIG)) {
	case BMAP_LEFT_CONTIG | BMAP_RIGHT_CONTIG:
		/*
		 * New allocation is contiguous with real allocations on the
		 * left and on the right.
		 * Merge all three into a single extent record.
		 */
		left.br_blockcount += new->br_blockcount + right.br_blockcount;

		xfs_iext_remove(ip, icur, state);
		xfs_iext_prev(ifp, icur);
		xfs_iext_update_extent(ip, state, icur, &left);

		XFS_IFORK_NEXT_SET(ip, whichfork,
			XFS_IFORK_NEXTENTS(ip, whichfork) - 1);
		if (cur == NULL) {
			rval = XFS_ILOG_CORE | xfs_ilog_fext(whichfork);
		} else {
			rval = XFS_ILOG_CORE;
			error = xfs_bmbt_lookup_eq(cur, &right, &i);
			if (error)
				goto done;
			XFS_WANT_CORRUPTED_GOTO(mp, i == 1, done);
			error = xfs_btree_delete(cur, &i);
			if (error)
				goto done;
			XFS_WANT_CORRUPTED_GOTO(mp, i == 1, done);
			error = xfs_btree_decrement(cur, 0, &i);
			if (error)
				goto done;
			XFS_WANT_CORRUPTED_GOTO(mp, i == 1, done);
			error = xfs_bmbt_update(cur, &left);
			if (error)
				goto done;
		}
		break;

	case BMAP_LEFT_CONTIG:
		/*
		 * New allocation is contiguous with a real allocation
		 * on the left.
		 * Merge the new allocation with the left neighbor.
		 */
		old = left;
		left.br_blockcount += new->br_blockcount;

		xfs_iext_prev(ifp, icur);
		xfs_iext_update_extent(ip, state, icur, &left);

		if (cur == NULL) {
			rval = xfs_ilog_fext(whichfork);
		} else {
			rval = 0;
			error = xfs_bmbt_lookup_eq(cur, &old, &i);
			if (error)
				goto done;
			XFS_WANT_CORRUPTED_GOTO(mp, i == 1, done);
			error = xfs_bmbt_update(cur, &left);
			if (error)
				goto done;
		}
		break;

	case BMAP_RIGHT_CONTIG:
		/*
		 * New allocation is contiguous with a real allocation
		 * on the right.
		 * Merge the new allocation with the right neighbor.
		 */
		old = right;

		right.br_startoff = new->br_startoff;
		right.br_startblock = new->br_startblock;
		right.br_blockcount += new->br_blockcount;
		xfs_iext_update_extent(ip, state, icur, &right);

		if (cur == NULL) {
			rval = xfs_ilog_fext(whichfork);
		} else {
			rval = 0;
			error = xfs_bmbt_lookup_eq(cur, &old, &i);
			if (error)
				goto done;
			XFS_WANT_CORRUPTED_GOTO(mp, i == 1, done);
			error = xfs_bmbt_update(cur, &right);
			if (error)
				goto done;
		}
		break;

	case 0:
		/*
		 * New allocation is not contiguous with another
		 * real allocation.
		 * Insert a new entry.
		 */
		xfs_iext_insert(ip, icur, new, state);
		XFS_IFORK_NEXT_SET(ip, whichfork,
			XFS_IFORK_NEXTENTS(ip, whichfork) + 1);
		if (cur == NULL) {
			rval = XFS_ILOG_CORE | xfs_ilog_fext(whichfork);
		} else {
			rval = XFS_ILOG_CORE;
			error = xfs_bmbt_lookup_eq(cur, new, &i);
			if (error)
				goto done;
			XFS_WANT_CORRUPTED_GOTO(mp, i == 0, done);
			error = xfs_btree_insert(cur, &i);
			if (error)
				goto done;
			XFS_WANT_CORRUPTED_GOTO(mp, i == 1, done);
		}
		break;
	}

	/* add reverse mapping */
	error = xfs_rmap_map_extent(mp, dfops, ip, whichfork, new);
	if (error)
		goto done;

	/* convert to a btree if necessary */
	if (xfs_bmap_needs_btree(ip, whichfork)) {
		int	tmp_logflags;	/* partial log flag return val */

		ASSERT(cur == NULL);
		error = xfs_bmap_extents_to_btree(tp, ip, first, dfops, curp,
				0, &tmp_logflags, whichfork);
		*logflagsp |= tmp_logflags;
		cur = *curp;
		if (error)
			goto done;
	}

	/* clear out the allocated field, done with it now in any case. */
	if (cur)
		cur->bc_private.b.allocated = 0;

	xfs_bmap_check_leaf_extents(cur, ip, whichfork);
done:
	*logflagsp |= rval;
	return error;
}
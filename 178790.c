xfs_bmap_add_extent_hole_delay(
	xfs_inode_t		*ip,	/* incore inode pointer */
	int			whichfork,
	struct xfs_iext_cursor	*icur,
	xfs_bmbt_irec_t		*new)	/* new data to add to file extents */
{
	xfs_ifork_t		*ifp;	/* inode fork pointer */
	xfs_bmbt_irec_t		left;	/* left neighbor extent entry */
	xfs_filblks_t		newlen=0;	/* new indirect size */
	xfs_filblks_t		oldlen=0;	/* old indirect size */
	xfs_bmbt_irec_t		right;	/* right neighbor extent entry */
	int			state = xfs_bmap_fork_to_state(whichfork);
	xfs_filblks_t		temp;	 /* temp for indirect calculations */

	ifp = XFS_IFORK_PTR(ip, whichfork);
	ASSERT(isnullstartblock(new->br_startblock));

	/*
	 * Check and set flags if this segment has a left neighbor
	 */
	if (xfs_iext_peek_prev_extent(ifp, icur, &left)) {
		state |= BMAP_LEFT_VALID;
		if (isnullstartblock(left.br_startblock))
			state |= BMAP_LEFT_DELAY;
	}

	/*
	 * Check and set flags if the current (right) segment exists.
	 * If it doesn't exist, we're converting the hole at end-of-file.
	 */
	if (xfs_iext_get_extent(ifp, icur, &right)) {
		state |= BMAP_RIGHT_VALID;
		if (isnullstartblock(right.br_startblock))
			state |= BMAP_RIGHT_DELAY;
	}

	/*
	 * Set contiguity flags on the left and right neighbors.
	 * Don't let extents get too large, even if the pieces are contiguous.
	 */
	if ((state & BMAP_LEFT_VALID) && (state & BMAP_LEFT_DELAY) &&
	    left.br_startoff + left.br_blockcount == new->br_startoff &&
	    left.br_blockcount + new->br_blockcount <= MAXEXTLEN)
		state |= BMAP_LEFT_CONTIG;

	if ((state & BMAP_RIGHT_VALID) && (state & BMAP_RIGHT_DELAY) &&
	    new->br_startoff + new->br_blockcount == right.br_startoff &&
	    new->br_blockcount + right.br_blockcount <= MAXEXTLEN &&
	    (!(state & BMAP_LEFT_CONTIG) ||
	     (left.br_blockcount + new->br_blockcount +
	      right.br_blockcount <= MAXEXTLEN)))
		state |= BMAP_RIGHT_CONTIG;

	/*
	 * Switch out based on the contiguity flags.
	 */
	switch (state & (BMAP_LEFT_CONTIG | BMAP_RIGHT_CONTIG)) {
	case BMAP_LEFT_CONTIG | BMAP_RIGHT_CONTIG:
		/*
		 * New allocation is contiguous with delayed allocations
		 * on the left and on the right.
		 * Merge all three into a single extent record.
		 */
		temp = left.br_blockcount + new->br_blockcount +
			right.br_blockcount;

		oldlen = startblockval(left.br_startblock) +
			startblockval(new->br_startblock) +
			startblockval(right.br_startblock);
		newlen = XFS_FILBLKS_MIN(xfs_bmap_worst_indlen(ip, temp),
					 oldlen);
		left.br_startblock = nullstartblock(newlen);
		left.br_blockcount = temp;

		xfs_iext_remove(ip, icur, state);
		xfs_iext_prev(ifp, icur);
		xfs_iext_update_extent(ip, state, icur, &left);
		break;

	case BMAP_LEFT_CONTIG:
		/*
		 * New allocation is contiguous with a delayed allocation
		 * on the left.
		 * Merge the new allocation with the left neighbor.
		 */
		temp = left.br_blockcount + new->br_blockcount;

		oldlen = startblockval(left.br_startblock) +
			startblockval(new->br_startblock);
		newlen = XFS_FILBLKS_MIN(xfs_bmap_worst_indlen(ip, temp),
					 oldlen);
		left.br_blockcount = temp;
		left.br_startblock = nullstartblock(newlen);

		xfs_iext_prev(ifp, icur);
		xfs_iext_update_extent(ip, state, icur, &left);
		break;

	case BMAP_RIGHT_CONTIG:
		/*
		 * New allocation is contiguous with a delayed allocation
		 * on the right.
		 * Merge the new allocation with the right neighbor.
		 */
		temp = new->br_blockcount + right.br_blockcount;
		oldlen = startblockval(new->br_startblock) +
			startblockval(right.br_startblock);
		newlen = XFS_FILBLKS_MIN(xfs_bmap_worst_indlen(ip, temp),
					 oldlen);
		right.br_startoff = new->br_startoff;
		right.br_startblock = nullstartblock(newlen);
		right.br_blockcount = temp;
		xfs_iext_update_extent(ip, state, icur, &right);
		break;

	case 0:
		/*
		 * New allocation is not contiguous with another
		 * delayed allocation.
		 * Insert a new entry.
		 */
		oldlen = newlen = 0;
		xfs_iext_insert(ip, icur, new, state);
		break;
	}
	if (oldlen != newlen) {
		ASSERT(oldlen > newlen);
		xfs_mod_fdblocks(ip->i_mount, (int64_t)(oldlen - newlen),
				 false);
		/*
		 * Nothing to do for disk quota accounting here.
		 */
	}
}
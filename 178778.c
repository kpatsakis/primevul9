xfs_bmap_del_extent_cow(
	struct xfs_inode	*ip,
	struct xfs_iext_cursor	*icur,
	struct xfs_bmbt_irec	*got,
	struct xfs_bmbt_irec	*del)
{
	struct xfs_mount	*mp = ip->i_mount;
	struct xfs_ifork	*ifp = XFS_IFORK_PTR(ip, XFS_COW_FORK);
	struct xfs_bmbt_irec	new;
	xfs_fileoff_t		del_endoff, got_endoff;
	int			state = BMAP_COWFORK;

	XFS_STATS_INC(mp, xs_del_exlist);

	del_endoff = del->br_startoff + del->br_blockcount;
	got_endoff = got->br_startoff + got->br_blockcount;

	ASSERT(del->br_blockcount > 0);
	ASSERT(got->br_startoff <= del->br_startoff);
	ASSERT(got_endoff >= del_endoff);
	ASSERT(!isnullstartblock(got->br_startblock));

	if (got->br_startoff == del->br_startoff)
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
		break;
	case BMAP_LEFT_FILLING:
		/*
		 * Deleting the first part of the extent.
		 */
		got->br_startoff = del_endoff;
		got->br_blockcount -= del->br_blockcount;
		got->br_startblock = del->br_startblock + del->br_blockcount;
		xfs_iext_update_extent(ip, state, icur, got);
		break;
	case BMAP_RIGHT_FILLING:
		/*
		 * Deleting the last part of the extent.
		 */
		got->br_blockcount -= del->br_blockcount;
		xfs_iext_update_extent(ip, state, icur, got);
		break;
	case 0:
		/*
		 * Deleting the middle of the extent.
		 */
		got->br_blockcount = del->br_startoff - got->br_startoff;

		new.br_startoff = del_endoff;
		new.br_blockcount = got_endoff - del_endoff;
		new.br_state = got->br_state;
		new.br_startblock = del->br_startblock + del->br_blockcount;

		xfs_iext_update_extent(ip, state, icur, got);
		xfs_iext_next(ifp, icur);
		xfs_iext_insert(ip, icur, &new, state);
		break;
	}
	ip->i_delayed_blks -= del->br_blockcount;
}
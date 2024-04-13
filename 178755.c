xfs_bmap_del_extent_delay(
	struct xfs_inode	*ip,
	int			whichfork,
	struct xfs_iext_cursor	*icur,
	struct xfs_bmbt_irec	*got,
	struct xfs_bmbt_irec	*del)
{
	struct xfs_mount	*mp = ip->i_mount;
	struct xfs_ifork	*ifp = XFS_IFORK_PTR(ip, whichfork);
	struct xfs_bmbt_irec	new;
	int64_t			da_old, da_new, da_diff = 0;
	xfs_fileoff_t		del_endoff, got_endoff;
	xfs_filblks_t		got_indlen, new_indlen, stolen;
	int			state = xfs_bmap_fork_to_state(whichfork);
	int			error = 0;
	bool			isrt;

	XFS_STATS_INC(mp, xs_del_exlist);

	isrt = (whichfork == XFS_DATA_FORK) && XFS_IS_REALTIME_INODE(ip);
	del_endoff = del->br_startoff + del->br_blockcount;
	got_endoff = got->br_startoff + got->br_blockcount;
	da_old = startblockval(got->br_startblock);
	da_new = 0;

	ASSERT(del->br_blockcount > 0);
	ASSERT(got->br_startoff <= del->br_startoff);
	ASSERT(got_endoff >= del_endoff);

	if (isrt) {
		uint64_t rtexts = XFS_FSB_TO_B(mp, del->br_blockcount);

		do_div(rtexts, mp->m_sb.sb_rextsize);
		xfs_mod_frextents(mp, rtexts);
	}

	/*
	 * Update the inode delalloc counter now and wait to update the
	 * sb counters as we might have to borrow some blocks for the
	 * indirect block accounting.
	 */
	error = xfs_trans_reserve_quota_nblks(NULL, ip,
			-((long)del->br_blockcount), 0,
			isrt ? XFS_QMOPT_RES_RTBLKS : XFS_QMOPT_RES_REGBLKS);
	if (error)
		return error;
	ip->i_delayed_blks -= del->br_blockcount;

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
		da_new = XFS_FILBLKS_MIN(xfs_bmap_worst_indlen(ip,
				got->br_blockcount), da_old);
		got->br_startblock = nullstartblock((int)da_new);
		xfs_iext_update_extent(ip, state, icur, got);
		break;
	case BMAP_RIGHT_FILLING:
		/*
		 * Deleting the last part of the extent.
		 */
		got->br_blockcount = got->br_blockcount - del->br_blockcount;
		da_new = XFS_FILBLKS_MIN(xfs_bmap_worst_indlen(ip,
				got->br_blockcount), da_old);
		got->br_startblock = nullstartblock((int)da_new);
		xfs_iext_update_extent(ip, state, icur, got);
		break;
	case 0:
		/*
		 * Deleting the middle of the extent.
		 *
		 * Distribute the original indlen reservation across the two new
		 * extents.  Steal blocks from the deleted extent if necessary.
		 * Stealing blocks simply fudges the fdblocks accounting below.
		 * Warn if either of the new indlen reservations is zero as this
		 * can lead to delalloc problems.
		 */
		got->br_blockcount = del->br_startoff - got->br_startoff;
		got_indlen = xfs_bmap_worst_indlen(ip, got->br_blockcount);

		new.br_blockcount = got_endoff - del_endoff;
		new_indlen = xfs_bmap_worst_indlen(ip, new.br_blockcount);

		WARN_ON_ONCE(!got_indlen || !new_indlen);
		stolen = xfs_bmap_split_indlen(da_old, &got_indlen, &new_indlen,
						       del->br_blockcount);

		got->br_startblock = nullstartblock((int)got_indlen);

		new.br_startoff = del_endoff;
		new.br_state = got->br_state;
		new.br_startblock = nullstartblock((int)new_indlen);

		xfs_iext_update_extent(ip, state, icur, got);
		xfs_iext_next(ifp, icur);
		xfs_iext_insert(ip, icur, &new, state);

		da_new = got_indlen + new_indlen - stolen;
		del->br_blockcount -= stolen;
		break;
	}

	ASSERT(da_old >= da_new);
	da_diff = da_old - da_new;
	if (!isrt)
		da_diff += del->br_blockcount;
	if (da_diff)
		xfs_mod_fdblocks(mp, da_diff, false);
	return error;
}
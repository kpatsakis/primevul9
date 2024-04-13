xfs_iread_extents(
	struct xfs_trans	*tp,
	struct xfs_inode	*ip,
	int			whichfork)
{
	struct xfs_mount	*mp = ip->i_mount;
	int			state = xfs_bmap_fork_to_state(whichfork);
	struct xfs_ifork	*ifp = XFS_IFORK_PTR(ip, whichfork);
	xfs_extnum_t		nextents = XFS_IFORK_NEXTENTS(ip, whichfork);
	struct xfs_btree_block	*block = ifp->if_broot;
	struct xfs_iext_cursor	icur;
	struct xfs_bmbt_irec	new;
	xfs_fsblock_t		bno;
	struct xfs_buf		*bp;
	xfs_extnum_t		i, j;
	int			level;
	__be64			*pp;
	int			error;

	ASSERT(xfs_isilocked(ip, XFS_ILOCK_EXCL));

	if (unlikely(XFS_IFORK_FORMAT(ip, whichfork) != XFS_DINODE_FMT_BTREE)) {
		XFS_ERROR_REPORT(__func__, XFS_ERRLEVEL_LOW, mp);
		return -EFSCORRUPTED;
	}

	/*
	 * Root level must use BMAP_BROOT_PTR_ADDR macro to get ptr out.
	 */
	level = be16_to_cpu(block->bb_level);
	ASSERT(level > 0);
	pp = XFS_BMAP_BROOT_PTR_ADDR(mp, block, 1, ifp->if_broot_bytes);
	bno = be64_to_cpu(*pp);

	/*
	 * Go down the tree until leaf level is reached, following the first
	 * pointer (leftmost) at each level.
	 */
	while (level-- > 0) {
		error = xfs_btree_read_bufl(mp, tp, bno, 0, &bp,
				XFS_BMAP_BTREE_REF, &xfs_bmbt_buf_ops);
		if (error)
			goto out;
		block = XFS_BUF_TO_BLOCK(bp);
		if (level == 0)
			break;
		pp = XFS_BMBT_PTR_ADDR(mp, block, 1, mp->m_bmap_dmxr[1]);
		bno = be64_to_cpu(*pp);
		XFS_WANT_CORRUPTED_GOTO(mp,
			xfs_verify_fsbno(mp, bno), out_brelse);
		xfs_trans_brelse(tp, bp);
	}

	/*
	 * Here with bp and block set to the leftmost leaf node in the tree.
	 */
	i = 0;
	xfs_iext_first(ifp, &icur);

	/*
	 * Loop over all leaf nodes.  Copy information to the extent records.
	 */
	for (;;) {
		xfs_bmbt_rec_t	*frp;
		xfs_fsblock_t	nextbno;
		xfs_extnum_t	num_recs;

		num_recs = xfs_btree_get_numrecs(block);
		if (unlikely(i + num_recs > nextents)) {
			ASSERT(i + num_recs <= nextents);
			xfs_warn(ip->i_mount,
				"corrupt dinode %Lu, (btree extents).",
				(unsigned long long) ip->i_ino);
			xfs_inode_verifier_error(ip, -EFSCORRUPTED,
					__func__, block, sizeof(*block),
					__this_address);
			error = -EFSCORRUPTED;
			goto out_brelse;
		}
		/*
		 * Read-ahead the next leaf block, if any.
		 */
		nextbno = be64_to_cpu(block->bb_u.l.bb_rightsib);
		if (nextbno != NULLFSBLOCK)
			xfs_btree_reada_bufl(mp, nextbno, 1,
					     &xfs_bmbt_buf_ops);
		/*
		 * Copy records into the extent records.
		 */
		frp = XFS_BMBT_REC_ADDR(mp, block, 1);
		for (j = 0; j < num_recs; j++, frp++, i++) {
			xfs_failaddr_t	fa;

			xfs_bmbt_disk_get_all(frp, &new);
			fa = xfs_bmap_validate_extent(ip, whichfork, &new);
			if (fa) {
				error = -EFSCORRUPTED;
				xfs_inode_verifier_error(ip, error,
						"xfs_iread_extents(2)",
						frp, sizeof(*frp), fa);
				goto out_brelse;
			}
			xfs_iext_insert(ip, &icur, &new, state);
			trace_xfs_read_extent(ip, &icur, state, _THIS_IP_);
			xfs_iext_next(ifp, &icur);
		}
		xfs_trans_brelse(tp, bp);
		bno = nextbno;
		/*
		 * If we've reached the end, stop.
		 */
		if (bno == NULLFSBLOCK)
			break;
		error = xfs_btree_read_bufl(mp, tp, bno, 0, &bp,
				XFS_BMAP_BTREE_REF, &xfs_bmbt_buf_ops);
		if (error)
			goto out;
		block = XFS_BUF_TO_BLOCK(bp);
	}

	if (i != XFS_IFORK_NEXTENTS(ip, whichfork)) {
		error = -EFSCORRUPTED;
		goto out;
	}
	ASSERT(i == xfs_iext_count(ifp));

	ifp->if_flags |= XFS_IFEXTENTS;
	return 0;

out_brelse:
	xfs_trans_brelse(tp, bp);
out:
	xfs_iext_destroy(ifp);
	return error;
}
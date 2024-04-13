xfs_bmap_check_leaf_extents(
	xfs_btree_cur_t		*cur,	/* btree cursor or null */
	xfs_inode_t		*ip,		/* incore inode pointer */
	int			whichfork)	/* data or attr fork */
{
	struct xfs_btree_block	*block;	/* current btree block */
	xfs_fsblock_t		bno;	/* block # of "block" */
	xfs_buf_t		*bp;	/* buffer for "block" */
	int			error;	/* error return value */
	xfs_extnum_t		i=0, j;	/* index into the extents list */
	xfs_ifork_t		*ifp;	/* fork structure */
	int			level;	/* btree level, for checking */
	xfs_mount_t		*mp;	/* file system mount structure */
	__be64			*pp;	/* pointer to block address */
	xfs_bmbt_rec_t		*ep;	/* pointer to current extent */
	xfs_bmbt_rec_t		last = {0, 0}; /* last extent in prev block */
	xfs_bmbt_rec_t		*nextp;	/* pointer to next extent */
	int			bp_release = 0;

	if (XFS_IFORK_FORMAT(ip, whichfork) != XFS_DINODE_FMT_BTREE) {
		return;
	}

	/* skip large extent count inodes */
	if (ip->i_d.di_nextents > 10000)
		return;

	bno = NULLFSBLOCK;
	mp = ip->i_mount;
	ifp = XFS_IFORK_PTR(ip, whichfork);
	block = ifp->if_broot;
	/*
	 * Root level must use BMAP_BROOT_PTR_ADDR macro to get ptr out.
	 */
	level = be16_to_cpu(block->bb_level);
	ASSERT(level > 0);
	xfs_check_block(block, mp, 1, ifp->if_broot_bytes);
	pp = XFS_BMAP_BROOT_PTR_ADDR(mp, block, 1, ifp->if_broot_bytes);
	bno = be64_to_cpu(*pp);

	ASSERT(bno != NULLFSBLOCK);
	ASSERT(XFS_FSB_TO_AGNO(mp, bno) < mp->m_sb.sb_agcount);
	ASSERT(XFS_FSB_TO_AGBNO(mp, bno) < mp->m_sb.sb_agblocks);

	/*
	 * Go down the tree until leaf level is reached, following the first
	 * pointer (leftmost) at each level.
	 */
	while (level-- > 0) {
		/* See if buf is in cur first */
		bp_release = 0;
		bp = xfs_bmap_get_bp(cur, XFS_FSB_TO_DADDR(mp, bno));
		if (!bp) {
			bp_release = 1;
			error = xfs_btree_read_bufl(mp, NULL, bno, 0, &bp,
						XFS_BMAP_BTREE_REF,
						&xfs_bmbt_buf_ops);
			if (error)
				goto error_norelse;
		}
		block = XFS_BUF_TO_BLOCK(bp);
		if (level == 0)
			break;

		/*
		 * Check this block for basic sanity (increasing keys and
		 * no duplicate blocks).
		 */

		xfs_check_block(block, mp, 0, 0);
		pp = XFS_BMBT_PTR_ADDR(mp, block, 1, mp->m_bmap_dmxr[1]);
		bno = be64_to_cpu(*pp);
		XFS_WANT_CORRUPTED_GOTO(mp,
					xfs_verify_fsbno(mp, bno), error0);
		if (bp_release) {
			bp_release = 0;
			xfs_trans_brelse(NULL, bp);
		}
	}

	/*
	 * Here with bp and block set to the leftmost leaf node in the tree.
	 */
	i = 0;

	/*
	 * Loop over all leaf nodes checking that all extents are in the right order.
	 */
	for (;;) {
		xfs_fsblock_t	nextbno;
		xfs_extnum_t	num_recs;


		num_recs = xfs_btree_get_numrecs(block);

		/*
		 * Read-ahead the next leaf block, if any.
		 */

		nextbno = be64_to_cpu(block->bb_u.l.bb_rightsib);

		/*
		 * Check all the extents to make sure they are OK.
		 * If we had a previous block, the last entry should
		 * conform with the first entry in this one.
		 */

		ep = XFS_BMBT_REC_ADDR(mp, block, 1);
		if (i) {
			ASSERT(xfs_bmbt_disk_get_startoff(&last) +
			       xfs_bmbt_disk_get_blockcount(&last) <=
			       xfs_bmbt_disk_get_startoff(ep));
		}
		for (j = 1; j < num_recs; j++) {
			nextp = XFS_BMBT_REC_ADDR(mp, block, j + 1);
			ASSERT(xfs_bmbt_disk_get_startoff(ep) +
			       xfs_bmbt_disk_get_blockcount(ep) <=
			       xfs_bmbt_disk_get_startoff(nextp));
			ep = nextp;
		}

		last = *ep;
		i += num_recs;
		if (bp_release) {
			bp_release = 0;
			xfs_trans_brelse(NULL, bp);
		}
		bno = nextbno;
		/*
		 * If we've reached the end, stop.
		 */
		if (bno == NULLFSBLOCK)
			break;

		bp_release = 0;
		bp = xfs_bmap_get_bp(cur, XFS_FSB_TO_DADDR(mp, bno));
		if (!bp) {
			bp_release = 1;
			error = xfs_btree_read_bufl(mp, NULL, bno, 0, &bp,
						XFS_BMAP_BTREE_REF,
						&xfs_bmbt_buf_ops);
			if (error)
				goto error_norelse;
		}
		block = XFS_BUF_TO_BLOCK(bp);
	}

	return;

error0:
	xfs_warn(mp, "%s: at error0", __func__);
	if (bp_release)
		xfs_trans_brelse(NULL, bp);
error_norelse:
	xfs_warn(mp, "%s: BAD after btree leaves for %d extents",
		__func__, i);
	panic("%s: CORRUPTED BTREE OR SOMETHING", __func__);
	return;
}
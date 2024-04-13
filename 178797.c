xfs_bmap_btalloc_accounting(
	struct xfs_bmalloca	*ap,
	struct xfs_alloc_arg	*args)
{
	if (ap->flags & XFS_BMAPI_COWFORK) {
		/*
		 * COW fork blocks are in-core only and thus are treated as
		 * in-core quota reservation (like delalloc blocks) even when
		 * converted to real blocks. The quota reservation is not
		 * accounted to disk until blocks are remapped to the data
		 * fork. So if these blocks were previously delalloc, we
		 * already have quota reservation and there's nothing to do
		 * yet.
		 */
		if (ap->wasdel)
			return;

		/*
		 * Otherwise, we've allocated blocks in a hole. The transaction
		 * has acquired in-core quota reservation for this extent.
		 * Rather than account these as real blocks, however, we reduce
		 * the transaction quota reservation based on the allocation.
		 * This essentially transfers the transaction quota reservation
		 * to that of a delalloc extent.
		 */
		ap->ip->i_delayed_blks += args->len;
		xfs_trans_mod_dquot_byino(ap->tp, ap->ip, XFS_TRANS_DQ_RES_BLKS,
				-(long)args->len);
		return;
	}

	/* data/attr fork only */
	ap->ip->i_d.di_nblocks += args->len;
	xfs_trans_log_inode(ap->tp, ap->ip, XFS_ILOG_CORE);
	if (ap->wasdel)
		ap->ip->i_delayed_blks -= args->len;
	xfs_trans_mod_dquot_byino(ap->tp, ap->ip,
		ap->wasdel ? XFS_TRANS_DQ_DELBCOUNT : XFS_TRANS_DQ_BCOUNT,
		args->len);
}
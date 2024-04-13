xfs_bmap_finish_one(
	struct xfs_trans		*tp,
	struct xfs_defer_ops		*dfops,
	struct xfs_inode		*ip,
	enum xfs_bmap_intent_type	type,
	int				whichfork,
	xfs_fileoff_t			startoff,
	xfs_fsblock_t			startblock,
	xfs_filblks_t			*blockcount,
	xfs_exntst_t			state)
{
	xfs_fsblock_t			firstfsb;
	int				error = 0;

	/*
	 * firstfsb is tied to the transaction lifetime and is used to
	 * ensure correct AG locking order and schedule work item
	 * continuations.  XFS_BUI_MAX_FAST_EXTENTS (== 1) restricts us
	 * to only making one bmap call per transaction, so it should
	 * be safe to have it as a local variable here.
	 */
	firstfsb = NULLFSBLOCK;

	trace_xfs_bmap_deferred(tp->t_mountp,
			XFS_FSB_TO_AGNO(tp->t_mountp, startblock), type,
			XFS_FSB_TO_AGBNO(tp->t_mountp, startblock),
			ip->i_ino, whichfork, startoff, *blockcount, state);

	if (WARN_ON_ONCE(whichfork != XFS_DATA_FORK))
		return -EFSCORRUPTED;

	if (XFS_TEST_ERROR(false, tp->t_mountp,
			XFS_ERRTAG_BMAP_FINISH_ONE))
		return -EIO;

	switch (type) {
	case XFS_BMAP_MAP:
		error = xfs_bmapi_remap(tp, ip, startoff, *blockcount,
				startblock, dfops);
		*blockcount = 0;
		break;
	case XFS_BMAP_UNMAP:
		error = __xfs_bunmapi(tp, ip, startoff, blockcount,
				XFS_BMAPI_REMAP, 1, &firstfsb, dfops);
		break;
	default:
		ASSERT(0);
		error = -EFSCORRUPTED;
	}

	return error;
}
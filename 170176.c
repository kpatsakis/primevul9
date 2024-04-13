xfs_iunlink_map_prev(
	struct xfs_trans	*tp,
	xfs_agnumber_t		agno,
	xfs_agino_t		head_agino,
	xfs_agino_t		target_agino,
	xfs_agino_t		*agino,
	struct xfs_imap		*imap,
	struct xfs_dinode	**dipp,
	struct xfs_buf		**bpp,
	struct xfs_perag	*pag)
{
	struct xfs_mount	*mp = tp->t_mountp;
	xfs_agino_t		next_agino;
	int			error;

	ASSERT(head_agino != target_agino);
	*bpp = NULL;

	/* See if our backref cache can find it faster. */
	*agino = xfs_iunlink_lookup_backref(pag, target_agino);
	if (*agino != NULLAGINO) {
		error = xfs_iunlink_map_ino(tp, agno, *agino, imap, dipp, bpp);
		if (error)
			return error;

		if (be32_to_cpu((*dipp)->di_next_unlinked) == target_agino)
			return 0;

		/*
		 * If we get here the cache contents were corrupt, so drop the
		 * buffer and fall back to walking the bucket list.
		 */
		xfs_trans_brelse(tp, *bpp);
		*bpp = NULL;
		WARN_ON_ONCE(1);
	}

	trace_xfs_iunlink_map_prev_fallback(mp, agno);

	/* Otherwise, walk the entire bucket until we find it. */
	next_agino = head_agino;
	while (next_agino != target_agino) {
		xfs_agino_t	unlinked_agino;

		if (*bpp)
			xfs_trans_brelse(tp, *bpp);

		*agino = next_agino;
		error = xfs_iunlink_map_ino(tp, agno, next_agino, imap, dipp,
				bpp);
		if (error)
			return error;

		unlinked_agino = be32_to_cpu((*dipp)->di_next_unlinked);
		/*
		 * Make sure this pointer is valid and isn't an obvious
		 * infinite loop.
		 */
		if (!xfs_verify_agino(mp, agno, unlinked_agino) ||
		    next_agino == unlinked_agino) {
			XFS_CORRUPTION_ERROR(__func__,
					XFS_ERRLEVEL_LOW, mp,
					*dipp, sizeof(**dipp));
			error = -EFSCORRUPTED;
			return error;
		}
		next_agino = unlinked_agino;
	}

	return 0;
}
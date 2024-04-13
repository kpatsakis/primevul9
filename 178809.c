xfs_bmap_btalloc_filestreams(
	struct xfs_bmalloca	*ap,
	struct xfs_alloc_arg	*args,
	xfs_extlen_t		*blen)
{
	struct xfs_mount	*mp = ap->ip->i_mount;
	xfs_agnumber_t		ag;
	int			notinit = 0;
	int			error;

	args->type = XFS_ALLOCTYPE_NEAR_BNO;
	args->total = ap->total;

	ag = XFS_FSB_TO_AGNO(mp, args->fsbno);
	if (ag == NULLAGNUMBER)
		ag = 0;

	error = xfs_bmap_longest_free_extent(args->tp, ag, blen, &notinit);
	if (error)
		return error;

	if (*blen < args->maxlen) {
		error = xfs_filestream_new_ag(ap, &ag);
		if (error)
			return error;

		error = xfs_bmap_longest_free_extent(args->tp, ag, blen,
						     &notinit);
		if (error)
			return error;

	}

	xfs_bmap_select_minlen(ap, args, blen, notinit);

	/*
	 * Set the failure fallback case to look in the selected AG as stream
	 * may have moved.
	 */
	ap->blkno = args->fsbno = XFS_AGB_TO_FSB(mp, ag, 0);
	return 0;
}
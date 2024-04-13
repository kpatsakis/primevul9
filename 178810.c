xfs_bmap_btalloc_nullfb(
	struct xfs_bmalloca	*ap,
	struct xfs_alloc_arg	*args,
	xfs_extlen_t		*blen)
{
	struct xfs_mount	*mp = ap->ip->i_mount;
	xfs_agnumber_t		ag, startag;
	int			notinit = 0;
	int			error;

	args->type = XFS_ALLOCTYPE_START_BNO;
	args->total = ap->total;

	startag = ag = XFS_FSB_TO_AGNO(mp, args->fsbno);
	if (startag == NULLAGNUMBER)
		startag = ag = 0;

	while (*blen < args->maxlen) {
		error = xfs_bmap_longest_free_extent(args->tp, ag, blen,
						     &notinit);
		if (error)
			return error;

		if (++ag == mp->m_sb.sb_agcount)
			ag = 0;
		if (ag == startag)
			break;
	}

	xfs_bmap_select_minlen(ap, args, blen, notinit);
	return 0;
}
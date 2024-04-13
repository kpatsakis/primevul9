xfs_bmap_longest_free_extent(
	struct xfs_trans	*tp,
	xfs_agnumber_t		ag,
	xfs_extlen_t		*blen,
	int			*notinit)
{
	struct xfs_mount	*mp = tp->t_mountp;
	struct xfs_perag	*pag;
	xfs_extlen_t		longest;
	int			error = 0;

	pag = xfs_perag_get(mp, ag);
	if (!pag->pagf_init) {
		error = xfs_alloc_pagf_init(mp, tp, ag, XFS_ALLOC_FLAG_TRYLOCK);
		if (error)
			goto out;

		if (!pag->pagf_init) {
			*notinit = 1;
			goto out;
		}
	}

	longest = xfs_alloc_longest_free_extent(pag,
				xfs_alloc_min_freelist(mp, pag),
				xfs_ag_resv_needed(pag, XFS_AG_RESV_NONE));
	if (*blen < longest)
		*blen = longest;

out:
	xfs_perag_put(pag);
	return error;
}
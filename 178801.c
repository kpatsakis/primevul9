__xfs_bmap_add(
	struct xfs_mount		*mp,
	struct xfs_defer_ops		*dfops,
	enum xfs_bmap_intent_type	type,
	struct xfs_inode		*ip,
	int				whichfork,
	struct xfs_bmbt_irec		*bmap)
{
	int				error;
	struct xfs_bmap_intent		*bi;

	trace_xfs_bmap_defer(mp,
			XFS_FSB_TO_AGNO(mp, bmap->br_startblock),
			type,
			XFS_FSB_TO_AGBNO(mp, bmap->br_startblock),
			ip->i_ino, whichfork,
			bmap->br_startoff,
			bmap->br_blockcount,
			bmap->br_state);

	bi = kmem_alloc(sizeof(struct xfs_bmap_intent), KM_SLEEP | KM_NOFS);
	INIT_LIST_HEAD(&bi->bi_list);
	bi->bi_type = type;
	bi->bi_owner = ip;
	bi->bi_whichfork = whichfork;
	bi->bi_bmap = *bmap;

	error = xfs_defer_ijoin(dfops, bi->bi_owner);
	if (error) {
		kmem_free(bi);
		return error;
	}

	xfs_defer_add(dfops, XFS_DEFER_OPS_TYPE_BMAP, &bi->bi_list);
	return 0;
}
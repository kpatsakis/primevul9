xfs_bmap_add_free(
	struct xfs_mount		*mp,
	struct xfs_defer_ops		*dfops,
	xfs_fsblock_t			bno,
	xfs_filblks_t			len,
	struct xfs_owner_info		*oinfo)
{
	struct xfs_extent_free_item	*new;		/* new element */
#ifdef DEBUG
	xfs_agnumber_t		agno;
	xfs_agblock_t		agbno;

	ASSERT(bno != NULLFSBLOCK);
	ASSERT(len > 0);
	ASSERT(len <= MAXEXTLEN);
	ASSERT(!isnullstartblock(bno));
	agno = XFS_FSB_TO_AGNO(mp, bno);
	agbno = XFS_FSB_TO_AGBNO(mp, bno);
	ASSERT(agno < mp->m_sb.sb_agcount);
	ASSERT(agbno < mp->m_sb.sb_agblocks);
	ASSERT(len < mp->m_sb.sb_agblocks);
	ASSERT(agbno + len <= mp->m_sb.sb_agblocks);
#endif
	ASSERT(xfs_bmap_free_item_zone != NULL);

	new = kmem_zone_alloc(xfs_bmap_free_item_zone, KM_SLEEP);
	new->xefi_startblock = bno;
	new->xefi_blockcount = (xfs_extlen_t)len;
	if (oinfo)
		new->xefi_oinfo = *oinfo;
	else
		xfs_rmap_skip_owner_update(&new->xefi_oinfo);
	trace_xfs_bmap_free_defer(mp, XFS_FSB_TO_AGNO(mp, bno), 0,
			XFS_FSB_TO_AGBNO(mp, bno), len);
	xfs_defer_add(dfops, XFS_DEFER_OPS_TYPE_FREE, &new->xefi_list);
}
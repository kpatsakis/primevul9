xfs_bmap_map_extent(
	struct xfs_mount	*mp,
	struct xfs_defer_ops	*dfops,
	struct xfs_inode	*ip,
	struct xfs_bmbt_irec	*PREV)
{
	if (!xfs_bmap_is_update_needed(PREV))
		return 0;

	return __xfs_bmap_add(mp, dfops, XFS_BMAP_MAP, ip,
			XFS_DATA_FORK, PREV);
}
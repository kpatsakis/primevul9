xfs_bmap_is_update_needed(
	struct xfs_bmbt_irec	*bmap)
{
	return  bmap->br_startblock != HOLESTARTBLOCK &&
		bmap->br_startblock != DELAYSTARTBLOCK;
}
xfs_bmap_isaeof(
	struct xfs_bmalloca	*bma,
	int			whichfork)
{
	struct xfs_bmbt_irec	rec;
	int			is_empty;
	int			error;

	bma->aeof = false;
	error = xfs_bmap_last_extent(NULL, bma->ip, whichfork, &rec,
				     &is_empty);
	if (error)
		return error;

	if (is_empty) {
		bma->aeof = true;
		return 0;
	}

	/*
	 * Check if we are allocation or past the last extent, or at least into
	 * the last delayed allocated extent.
	 */
	bma->aeof = bma->offset >= rec.br_startoff + rec.br_blockcount ||
		(bma->offset >= rec.br_startoff &&
		 isnullstartblock(rec.br_startblock));
	return 0;
}
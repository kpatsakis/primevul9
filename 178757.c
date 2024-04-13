xfs_bmse_can_merge(
	struct xfs_bmbt_irec	*left,	/* preceding extent */
	struct xfs_bmbt_irec	*got,	/* current extent to shift */
	xfs_fileoff_t		shift)	/* shift fsb */
{
	xfs_fileoff_t		startoff;

	startoff = got->br_startoff - shift;

	/*
	 * The extent, once shifted, must be adjacent in-file and on-disk with
	 * the preceding extent.
	 */
	if ((left->br_startoff + left->br_blockcount != startoff) ||
	    (left->br_startblock + left->br_blockcount != got->br_startblock) ||
	    (left->br_state != got->br_state) ||
	    (left->br_blockcount + got->br_blockcount > MAXEXTLEN))
		return false;

	return true;
}
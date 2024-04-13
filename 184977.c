xfs_attr3_leaf_inactive(
	struct xfs_trans	**trans,
	struct xfs_inode	*dp,
	struct xfs_buf		*bp)
{
	struct xfs_attr_leafblock *leaf;
	struct xfs_attr3_icleaf_hdr ichdr;
	struct xfs_attr_leaf_entry *entry;
	struct xfs_attr_leaf_name_remote *name_rmt;
	struct xfs_attr_inactive_list *list;
	struct xfs_attr_inactive_list *lp;
	int			error;
	int			count;
	int			size;
	int			tmp;
	int			i;

	leaf = bp->b_addr;
	xfs_attr3_leaf_hdr_from_disk(&ichdr, leaf);

	/*
	 * Count the number of "remote" value extents.
	 */
	count = 0;
	entry = xfs_attr3_leaf_entryp(leaf);
	for (i = 0; i < ichdr.count; entry++, i++) {
		if (be16_to_cpu(entry->nameidx) &&
		    ((entry->flags & XFS_ATTR_LOCAL) == 0)) {
			name_rmt = xfs_attr3_leaf_name_remote(leaf, i);
			if (name_rmt->valueblk)
				count++;
		}
	}

	/*
	 * If there are no "remote" values, we're done.
	 */
	if (count == 0) {
		xfs_trans_brelse(*trans, bp);
		return 0;
	}

	/*
	 * Allocate storage for a list of all the "remote" value extents.
	 */
	size = count * sizeof(xfs_attr_inactive_list_t);
	list = kmem_alloc(size, KM_SLEEP);

	/*
	 * Identify each of the "remote" value extents.
	 */
	lp = list;
	entry = xfs_attr3_leaf_entryp(leaf);
	for (i = 0; i < ichdr.count; entry++, i++) {
		if (be16_to_cpu(entry->nameidx) &&
		    ((entry->flags & XFS_ATTR_LOCAL) == 0)) {
			name_rmt = xfs_attr3_leaf_name_remote(leaf, i);
			if (name_rmt->valueblk) {
				lp->valueblk = be32_to_cpu(name_rmt->valueblk);
				lp->valuelen = XFS_B_TO_FSB(dp->i_mount,
						    be32_to_cpu(name_rmt->valuelen));
				lp++;
			}
		}
	}
	xfs_trans_brelse(*trans, bp);	/* unlock for trans. in freextent() */

	/*
	 * Invalidate each of the "remote" value extents.
	 */
	error = 0;
	for (lp = list, i = 0; i < count; i++, lp++) {
		tmp = xfs_attr3_leaf_freextent(trans, dp,
				lp->valueblk, lp->valuelen);

		if (error == 0)
			error = tmp;	/* save only the 1st errno */
	}

	kmem_free(list);
	return error;
}
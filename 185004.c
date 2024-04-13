xfs_attr3_root_inactive(
	struct xfs_trans	**trans,
	struct xfs_inode	*dp)
{
	struct xfs_da_blkinfo	*info;
	struct xfs_buf		*bp;
	xfs_daddr_t		blkno;
	int			error;

	/*
	 * Read block 0 to see what we have to work with.
	 * We only get here if we have extents, since we remove
	 * the extents in reverse order the extent containing
	 * block 0 must still be there.
	 */
	error = xfs_da3_node_read(*trans, dp, 0, -1, &bp, XFS_ATTR_FORK);
	if (error)
		return error;
	blkno = bp->b_bn;

	/*
	 * Invalidate the tree, even if the "tree" is only a single leaf block.
	 * This is a depth-first traversal!
	 */
	info = bp->b_addr;
	switch (info->magic) {
	case cpu_to_be16(XFS_DA_NODE_MAGIC):
	case cpu_to_be16(XFS_DA3_NODE_MAGIC):
		error = xfs_attr3_node_inactive(trans, dp, bp, 1);
		break;
	case cpu_to_be16(XFS_ATTR_LEAF_MAGIC):
	case cpu_to_be16(XFS_ATTR3_LEAF_MAGIC):
		error = xfs_attr3_leaf_inactive(trans, dp, bp);
		break;
	default:
		error = XFS_ERROR(EIO);
		xfs_trans_brelse(*trans, bp);
		break;
	}
	if (error)
		return error;

	/*
	 * Invalidate the incore copy of the root block.
	 */
	error = xfs_da_get_buf(*trans, dp, 0, blkno, &bp, XFS_ATTR_FORK);
	if (error)
		return error;
	xfs_trans_binval(*trans, bp);	/* remove from cache */
	/*
	 * Commit the invalidate and start the next transaction.
	 */
	error = xfs_trans_roll(trans, dp);

	return error;
}
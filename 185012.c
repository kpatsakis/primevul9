xfs_attr3_node_inactive(
	struct xfs_trans **trans,
	struct xfs_inode *dp,
	struct xfs_buf	*bp,
	int		level)
{
	xfs_da_blkinfo_t *info;
	xfs_da_intnode_t *node;
	xfs_dablk_t child_fsb;
	xfs_daddr_t parent_blkno, child_blkno;
	int error, i;
	struct xfs_buf *child_bp;
	struct xfs_da_node_entry *btree;
	struct xfs_da3_icnode_hdr ichdr;

	/*
	 * Since this code is recursive (gasp!) we must protect ourselves.
	 */
	if (level > XFS_DA_NODE_MAXDEPTH) {
		xfs_trans_brelse(*trans, bp);	/* no locks for later trans */
		return XFS_ERROR(EIO);
	}

	node = bp->b_addr;
	xfs_da3_node_hdr_from_disk(&ichdr, node);
	parent_blkno = bp->b_bn;
	if (!ichdr.count) {
		xfs_trans_brelse(*trans, bp);
		return 0;
	}
	btree = xfs_da3_node_tree_p(node);
	child_fsb = be32_to_cpu(btree[0].before);
	xfs_trans_brelse(*trans, bp);	/* no locks for later trans */

	/*
	 * If this is the node level just above the leaves, simply loop
	 * over the leaves removing all of them.  If this is higher up
	 * in the tree, recurse downward.
	 */
	for (i = 0; i < ichdr.count; i++) {
		/*
		 * Read the subsidiary block to see what we have to work with.
		 * Don't do this in a transaction.  This is a depth-first
		 * traversal of the tree so we may deal with many blocks
		 * before we come back to this one.
		 */
		error = xfs_da3_node_read(*trans, dp, child_fsb, -2, &child_bp,
						XFS_ATTR_FORK);
		if (error)
			return(error);
		if (child_bp) {
						/* save for re-read later */
			child_blkno = XFS_BUF_ADDR(child_bp);

			/*
			 * Invalidate the subtree, however we have to.
			 */
			info = child_bp->b_addr;
			switch (info->magic) {
			case cpu_to_be16(XFS_DA_NODE_MAGIC):
			case cpu_to_be16(XFS_DA3_NODE_MAGIC):
				error = xfs_attr3_node_inactive(trans, dp,
							child_bp, level + 1);
				break;
			case cpu_to_be16(XFS_ATTR_LEAF_MAGIC):
			case cpu_to_be16(XFS_ATTR3_LEAF_MAGIC):
				error = xfs_attr3_leaf_inactive(trans, dp,
							child_bp);
				break;
			default:
				error = XFS_ERROR(EIO);
				xfs_trans_brelse(*trans, child_bp);
				break;
			}
			if (error)
				return error;

			/*
			 * Remove the subsidiary block from the cache
			 * and from the log.
			 */
			error = xfs_da_get_buf(*trans, dp, 0, child_blkno,
				&child_bp, XFS_ATTR_FORK);
			if (error)
				return error;
			xfs_trans_binval(*trans, child_bp);
		}

		/*
		 * If we're not done, re-read the parent to get the next
		 * child block number.
		 */
		if (i + 1 < ichdr.count) {
			error = xfs_da3_node_read(*trans, dp, 0, parent_blkno,
						 &bp, XFS_ATTR_FORK);
			if (error)
				return error;
			child_fsb = be32_to_cpu(btree[i + 1].before);
			xfs_trans_brelse(*trans, bp);
		}
		/*
		 * Atomically commit the whole invalidate stuff.
		 */
		error = xfs_trans_roll(trans, dp);
		if (error)
			return  error;
	}

	return 0;
}
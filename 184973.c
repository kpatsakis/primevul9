xfs_attr3_leaf_to_node(
	struct xfs_da_args	*args)
{
	struct xfs_attr_leafblock *leaf;
	struct xfs_attr3_icleaf_hdr icleafhdr;
	struct xfs_attr_leaf_entry *entries;
	struct xfs_da_node_entry *btree;
	struct xfs_da3_icnode_hdr icnodehdr;
	struct xfs_da_intnode	*node;
	struct xfs_inode	*dp = args->dp;
	struct xfs_mount	*mp = dp->i_mount;
	struct xfs_buf		*bp1 = NULL;
	struct xfs_buf		*bp2 = NULL;
	xfs_dablk_t		blkno;
	int			error;

	trace_xfs_attr_leaf_to_node(args);

	error = xfs_da_grow_inode(args, &blkno);
	if (error)
		goto out;
	error = xfs_attr3_leaf_read(args->trans, dp, 0, -1, &bp1);
	if (error)
		goto out;

	error = xfs_da_get_buf(args->trans, dp, blkno, -1, &bp2, XFS_ATTR_FORK);
	if (error)
		goto out;

	/* copy leaf to new buffer, update identifiers */
	xfs_trans_buf_set_type(args->trans, bp2, XFS_BLFT_ATTR_LEAF_BUF);
	bp2->b_ops = bp1->b_ops;
	memcpy(bp2->b_addr, bp1->b_addr, XFS_LBSIZE(mp));
	if (xfs_sb_version_hascrc(&mp->m_sb)) {
		struct xfs_da3_blkinfo *hdr3 = bp2->b_addr;
		hdr3->blkno = cpu_to_be64(bp2->b_bn);
	}
	xfs_trans_log_buf(args->trans, bp2, 0, XFS_LBSIZE(mp) - 1);

	/*
	 * Set up the new root node.
	 */
	error = xfs_da3_node_create(args, 0, 1, &bp1, XFS_ATTR_FORK);
	if (error)
		goto out;
	node = bp1->b_addr;
	xfs_da3_node_hdr_from_disk(&icnodehdr, node);
	btree = xfs_da3_node_tree_p(node);

	leaf = bp2->b_addr;
	xfs_attr3_leaf_hdr_from_disk(&icleafhdr, leaf);
	entries = xfs_attr3_leaf_entryp(leaf);

	/* both on-disk, don't endian-flip twice */
	btree[0].hashval = entries[icleafhdr.count - 1].hashval;
	btree[0].before = cpu_to_be32(blkno);
	icnodehdr.count = 1;
	xfs_da3_node_hdr_to_disk(node, &icnodehdr);
	xfs_trans_log_buf(args->trans, bp1, 0, XFS_LBSIZE(mp) - 1);
	error = 0;
out:
	return error;
}
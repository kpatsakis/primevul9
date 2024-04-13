xfs_attr_leaf_order(
	struct xfs_buf	*leaf1_bp,
	struct xfs_buf	*leaf2_bp)
{
	struct xfs_attr3_icleaf_hdr ichdr1;
	struct xfs_attr3_icleaf_hdr ichdr2;

	xfs_attr3_leaf_hdr_from_disk(&ichdr1, leaf1_bp->b_addr);
	xfs_attr3_leaf_hdr_from_disk(&ichdr2, leaf2_bp->b_addr);
	return xfs_attr3_leaf_order(leaf1_bp, &ichdr1, leaf2_bp, &ichdr2);
}
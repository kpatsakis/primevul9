xfs_attr3_leaf_verify(
	struct xfs_buf		*bp)
{
	struct xfs_mount	*mp = bp->b_target->bt_mount;
	struct xfs_attr_leafblock *leaf = bp->b_addr;
	struct xfs_attr3_icleaf_hdr ichdr;

	xfs_attr3_leaf_hdr_from_disk(&ichdr, leaf);

	if (xfs_sb_version_hascrc(&mp->m_sb)) {
		struct xfs_da3_node_hdr *hdr3 = bp->b_addr;

		if (ichdr.magic != XFS_ATTR3_LEAF_MAGIC)
			return false;

		if (!uuid_equal(&hdr3->info.uuid, &mp->m_sb.sb_uuid))
			return false;
		if (be64_to_cpu(hdr3->info.blkno) != bp->b_bn)
			return false;
	} else {
		if (ichdr.magic != XFS_ATTR_LEAF_MAGIC)
			return false;
	}
	if (ichdr.count == 0)
		return false;

	/* XXX: need to range check rest of attr header values */
	/* XXX: hash order check? */

	return true;
}
xfs_attr_leaf_lasthash(
	struct xfs_buf	*bp,
	int		*count)
{
	struct xfs_attr3_icleaf_hdr ichdr;
	struct xfs_attr_leaf_entry *entries;

	xfs_attr3_leaf_hdr_from_disk(&ichdr, bp->b_addr);
	entries = xfs_attr3_leaf_entryp(bp->b_addr);
	if (count)
		*count = ichdr.count;
	if (!ichdr.count)
		return 0;
	return be32_to_cpu(entries[ichdr.count - 1].hashval);
}
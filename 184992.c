xfs_attr3_leaf_hdr_from_disk(
	struct xfs_attr3_icleaf_hdr	*to,
	struct xfs_attr_leafblock	*from)
{
	int	i;

	ASSERT(from->hdr.info.magic == cpu_to_be16(XFS_ATTR_LEAF_MAGIC) ||
	       from->hdr.info.magic == cpu_to_be16(XFS_ATTR3_LEAF_MAGIC));

	if (from->hdr.info.magic == cpu_to_be16(XFS_ATTR3_LEAF_MAGIC)) {
		struct xfs_attr3_leaf_hdr *hdr3 = (struct xfs_attr3_leaf_hdr *)from;

		to->forw = be32_to_cpu(hdr3->info.hdr.forw);
		to->back = be32_to_cpu(hdr3->info.hdr.back);
		to->magic = be16_to_cpu(hdr3->info.hdr.magic);
		to->count = be16_to_cpu(hdr3->count);
		to->usedbytes = be16_to_cpu(hdr3->usedbytes);
		to->firstused = be16_to_cpu(hdr3->firstused);
		to->holes = hdr3->holes;

		for (i = 0; i < XFS_ATTR_LEAF_MAPSIZE; i++) {
			to->freemap[i].base = be16_to_cpu(hdr3->freemap[i].base);
			to->freemap[i].size = be16_to_cpu(hdr3->freemap[i].size);
		}
		return;
	}
	to->forw = be32_to_cpu(from->hdr.info.forw);
	to->back = be32_to_cpu(from->hdr.info.back);
	to->magic = be16_to_cpu(from->hdr.info.magic);
	to->count = be16_to_cpu(from->hdr.count);
	to->usedbytes = be16_to_cpu(from->hdr.usedbytes);
	to->firstused = be16_to_cpu(from->hdr.firstused);
	to->holes = from->hdr.holes;

	for (i = 0; i < XFS_ATTR_LEAF_MAPSIZE; i++) {
		to->freemap[i].base = be16_to_cpu(from->hdr.freemap[i].base);
		to->freemap[i].size = be16_to_cpu(from->hdr.freemap[i].size);
	}
}
xfs_attr3_leaf_hdr_to_disk(
	struct xfs_attr_leafblock	*to,
	struct xfs_attr3_icleaf_hdr	*from)
{
	int	i;

	ASSERT(from->magic == XFS_ATTR_LEAF_MAGIC ||
	       from->magic == XFS_ATTR3_LEAF_MAGIC);

	if (from->magic == XFS_ATTR3_LEAF_MAGIC) {
		struct xfs_attr3_leaf_hdr *hdr3 = (struct xfs_attr3_leaf_hdr *)to;

		hdr3->info.hdr.forw = cpu_to_be32(from->forw);
		hdr3->info.hdr.back = cpu_to_be32(from->back);
		hdr3->info.hdr.magic = cpu_to_be16(from->magic);
		hdr3->count = cpu_to_be16(from->count);
		hdr3->usedbytes = cpu_to_be16(from->usedbytes);
		hdr3->firstused = cpu_to_be16(from->firstused);
		hdr3->holes = from->holes;
		hdr3->pad1 = 0;

		for (i = 0; i < XFS_ATTR_LEAF_MAPSIZE; i++) {
			hdr3->freemap[i].base = cpu_to_be16(from->freemap[i].base);
			hdr3->freemap[i].size = cpu_to_be16(from->freemap[i].size);
		}
		return;
	}
	to->hdr.info.forw = cpu_to_be32(from->forw);
	to->hdr.info.back = cpu_to_be32(from->back);
	to->hdr.info.magic = cpu_to_be16(from->magic);
	to->hdr.count = cpu_to_be16(from->count);
	to->hdr.usedbytes = cpu_to_be16(from->usedbytes);
	to->hdr.firstused = cpu_to_be16(from->firstused);
	to->hdr.holes = from->holes;
	to->hdr.pad1 = 0;

	for (i = 0; i < XFS_ATTR_LEAF_MAPSIZE; i++) {
		to->hdr.freemap[i].base = cpu_to_be16(from->freemap[i].base);
		to->hdr.freemap[i].size = cpu_to_be16(from->freemap[i].size);
	}
}
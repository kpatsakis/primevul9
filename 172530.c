static int check_defrag_in_cache(struct inode *inode, u64 offset, u32 thresh)
{
	struct extent_io_tree *io_tree = &BTRFS_I(inode)->io_tree;
	struct extent_map *em = NULL;
	struct extent_map_tree *em_tree = &BTRFS_I(inode)->extent_tree;
	u64 end;

	read_lock(&em_tree->lock);
	em = lookup_extent_mapping(em_tree, offset, PAGE_SIZE);
	read_unlock(&em_tree->lock);

	if (em) {
		end = extent_map_end(em);
		free_extent_map(em);
		if (end - offset > thresh)
			return 0;
	}
	/* if we already have a nice delalloc here, just stop */
	thresh /= 2;
	end = count_range_bits(io_tree, &offset, offset + thresh,
			       thresh, EXTENT_DELALLOC, 1);
	if (end >= thresh)
		return 0;
	return 1;
}
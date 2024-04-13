static struct extent_map *defrag_lookup_extent(struct inode *inode, u64 start)
{
	struct extent_map_tree *em_tree = &BTRFS_I(inode)->extent_tree;
	struct extent_io_tree *io_tree = &BTRFS_I(inode)->io_tree;
	struct extent_map *em;
	u64 len = PAGE_SIZE;

	/*
	 * hopefully we have this extent in the tree already, try without
	 * the full extent lock
	 */
	read_lock(&em_tree->lock);
	em = lookup_extent_mapping(em_tree, start, len);
	read_unlock(&em_tree->lock);

	if (!em) {
		struct extent_state *cached = NULL;
		u64 end = start + len - 1;

		/* get the big lock and read metadata off disk */
		lock_extent_bits(io_tree, start, end, &cached);
		em = btrfs_get_extent(BTRFS_I(inode), NULL, 0, start, len, 0);
		unlock_extent_cached(io_tree, start, end, &cached);

		if (IS_ERR(em))
			return NULL;
	}

	return em;
}
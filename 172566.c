static bool defrag_check_next_extent(struct inode *inode, struct extent_map *em)
{
	struct extent_map *next;
	bool ret = true;

	/* this is the last extent */
	if (em->start + em->len >= i_size_read(inode))
		return false;

	next = defrag_lookup_extent(inode, em->start + em->len);
	if (!next || next->block_start >= EXTENT_MAP_LAST_BYTE)
		ret = false;
	else if ((em->block_start + em->block_len == next->block_start) &&
		 (em->block_len > SZ_128K && next->block_len > SZ_128K))
		ret = false;

	free_extent_map(next);
	return ret;
}
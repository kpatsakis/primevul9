static int ext4_iomap_begin_report(struct inode *inode, loff_t offset,
				   loff_t length, unsigned int flags,
				   struct iomap *iomap, struct iomap *srcmap)
{
	int ret;
	bool delalloc = false;
	struct ext4_map_blocks map;
	u8 blkbits = inode->i_blkbits;

	if ((offset >> blkbits) > EXT4_MAX_LOGICAL_BLOCK)
		return -EINVAL;

	if (ext4_has_inline_data(inode)) {
		ret = ext4_inline_data_iomap(inode, iomap);
		if (ret != -EAGAIN) {
			if (ret == 0 && offset >= iomap->length)
				ret = -ENOENT;
			return ret;
		}
	}

	/*
	 * Calculate the first and last logical block respectively.
	 */
	map.m_lblk = offset >> blkbits;
	map.m_len = min_t(loff_t, (offset + length - 1) >> blkbits,
			  EXT4_MAX_LOGICAL_BLOCK) - map.m_lblk + 1;

	ret = ext4_map_blocks(NULL, inode, &map, 0);
	if (ret < 0)
		return ret;
	if (ret == 0)
		delalloc = ext4_iomap_is_delalloc(inode, &map);

	ext4_set_iomap(inode, iomap, &map, offset, length);
	if (delalloc && iomap->type == IOMAP_HOLE)
		iomap->type = IOMAP_DELALLOC;

	return 0;
}
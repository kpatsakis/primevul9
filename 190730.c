bool f2fs_overwrite_io(struct inode *inode, loff_t pos, size_t len)
{
	struct f2fs_map_blocks map;
	block_t last_lblk;
	int err;

	if (pos + len > i_size_read(inode))
		return false;

	map.m_lblk = F2FS_BYTES_TO_BLK(pos);
	map.m_next_pgofs = NULL;
	map.m_next_extent = NULL;
	map.m_seg_type = NO_CHECK_TYPE;
	map.m_may_create = false;
	last_lblk = F2FS_BLK_ALIGN(pos + len);

	while (map.m_lblk < last_lblk) {
		map.m_len = last_lblk - map.m_lblk;
		err = f2fs_map_blocks(inode, &map, 0, F2FS_GET_BLOCK_DEFAULT);
		if (err || map.m_len == 0)
			return false;
		map.m_lblk += map.m_len;
	}
	return true;
}
static void f2fs_write_failed(struct address_space *mapping, loff_t to)
{
	struct inode *inode = mapping->host;
	loff_t i_size = i_size_read(inode);

	if (to > i_size) {
		down_write(&F2FS_I(inode)->i_gc_rwsem[WRITE]);
		down_write(&F2FS_I(inode)->i_mmap_sem);

		truncate_pagecache(inode, i_size);
		if (!IS_NOQUOTA(inode))
			f2fs_truncate_blocks(inode, i_size, true);

		up_write(&F2FS_I(inode)->i_mmap_sem);
		up_write(&F2FS_I(inode)->i_gc_rwsem[WRITE]);
	}
}
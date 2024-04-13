loff_t btrfs_remap_file_range(struct file *src_file, loff_t off,
		struct file *dst_file, loff_t destoff, loff_t len,
		unsigned int remap_flags)
{
	struct inode *src_inode = file_inode(src_file);
	struct inode *dst_inode = file_inode(dst_file);
	bool same_inode = dst_inode == src_inode;
	int ret;

	if (remap_flags & ~(REMAP_FILE_DEDUP | REMAP_FILE_ADVISORY))
		return -EINVAL;

	ret = btrfs_remap_file_range_prep(src_file, off, dst_file, destoff,
					  &len, remap_flags);
	if (ret < 0 || len == 0)
		return ret;

	if (remap_flags & REMAP_FILE_DEDUP)
		ret = btrfs_extent_same(src_inode, off, len, dst_inode, destoff);
	else
		ret = btrfs_clone_files(dst_file, src_file, off, len, destoff);

	if (same_inode)
		inode_unlock(src_inode);
	else
		btrfs_double_inode_unlock(src_inode, dst_inode);

	return ret < 0 ? ret : len;
}
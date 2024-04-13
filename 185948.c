int generic_block_fiemap(struct inode *inode,
			 struct fiemap_extent_info *fieinfo, u64 start,
			 u64 len, get_block_t *get_block)
{
	int ret;
	mutex_lock(&inode->i_mutex);
	ret = __generic_block_fiemap(inode, fieinfo, start, len, get_block);
	mutex_unlock(&inode->i_mutex);
	return ret;
}
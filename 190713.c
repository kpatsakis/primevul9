static inline int F2FS_HAS_BLOCKS(struct inode *inode)
{
	block_t xattr_block = F2FS_I(inode)->i_xattr_nid ? 1 : 0;

	return (inode->i_blocks >> F2FS_LOG_SECTORS_PER_BLOCK) > xattr_block;
}
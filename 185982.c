static inline loff_t blk_to_logical(struct inode *inode, sector_t blk)
{
	return (blk << inode->i_blkbits);
}
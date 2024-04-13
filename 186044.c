static inline sector_t logical_to_blk(struct inode *inode, loff_t offset)
{
	return (offset >> inode->i_blkbits);
}
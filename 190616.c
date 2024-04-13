static inline unsigned int addrs_per_block(struct inode *inode)
{
	return ALIGN_DOWN(DEF_ADDRS_PER_BLOCK, 1);
}
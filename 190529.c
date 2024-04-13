static inline unsigned int addrs_per_inode(struct inode *inode)
{
	unsigned int addrs = CUR_ADDRS_PER_INODE(inode) -
				get_inline_xattr_addrs(inode);
	return ALIGN_DOWN(addrs, 1);
}
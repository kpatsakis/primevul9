static inline void *inline_xattr_addr(struct inode *inode, struct page *page)
{
	struct f2fs_inode *ri = F2FS_INODE(page);

	return (void *)&(ri->i_addr[DEF_ADDRS_PER_INODE -
					get_inline_xattr_addrs(inode)]);
}
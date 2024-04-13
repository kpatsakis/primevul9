static inline int get_inline_xattr_addrs(struct inode *inode)
{
	return F2FS_I(inode)->i_inline_xattr_size;
}
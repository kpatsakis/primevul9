static inline int get_extra_isize(struct inode *inode)
{
	return F2FS_I(inode)->i_extra_isize / sizeof(__le32);
}
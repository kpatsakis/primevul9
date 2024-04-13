static inline int is_file(struct inode *inode, int type)
{
	return F2FS_I(inode)->i_advise & type;
}
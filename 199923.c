static int bad_inode_setattr(struct dentry *direntry, struct iattr *attrs)
{
	return -EIO;
}
static int bad_inode_permission(struct inode *inode, int mask,
			struct nameidata *nd)
{
	return -EIO;
}
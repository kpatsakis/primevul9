static void btrfs_double_inode_unlock(struct inode *inode1, struct inode *inode2)
{
	inode_unlock(inode1);
	inode_unlock(inode2);
}
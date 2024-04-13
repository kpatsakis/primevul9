static void btrfs_double_inode_lock(struct inode *inode1, struct inode *inode2)
{
	if (inode1 < inode2)
		swap(inode1, inode2);

	inode_lock_nested(inode1, I_MUTEX_PARENT);
	inode_lock_nested(inode2, I_MUTEX_CHILD);
}
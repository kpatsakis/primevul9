static void btrfs_double_extent_lock(struct inode *inode1, u64 loff1,
				     struct inode *inode2, u64 loff2, u64 len)
{
	if (inode1 < inode2) {
		swap(inode1, inode2);
		swap(loff1, loff2);
	} else if (inode1 == inode2 && loff2 < loff1) {
		swap(loff1, loff2);
	}
	lock_extent(&BTRFS_I(inode1)->io_tree, loff1, loff1 + len - 1);
	lock_extent(&BTRFS_I(inode2)->io_tree, loff2, loff2 + len - 1);
}
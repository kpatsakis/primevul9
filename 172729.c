static void btrfs_double_extent_unlock(struct inode *inode1, u64 loff1,
				       struct inode *inode2, u64 loff2, u64 len)
{
	unlock_extent(&BTRFS_I(inode1)->io_tree, loff1, loff1 + len - 1);
	unlock_extent(&BTRFS_I(inode2)->io_tree, loff2, loff2 + len - 1);
}
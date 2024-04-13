static int ext4_nfs_commit_metadata(struct inode *inode)
{
	struct writeback_control wbc = {
		.sync_mode = WB_SYNC_ALL
	};

	trace_ext4_nfs_commit_metadata(inode);
	return ext4_write_inode(inode, &wbc);
}
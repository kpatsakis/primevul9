static unsigned int btrfs_mask_fsflags_for_type(struct inode *inode,
		unsigned int flags)
{
	if (S_ISDIR(inode->i_mode))
		return flags;
	else if (S_ISREG(inode->i_mode))
		return flags & ~FS_DIRSYNC_FL;
	else
		return flags & (FS_NODUMP_FL | FS_NOATIME_FL);
}
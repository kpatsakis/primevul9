void ext4_set_inode_flags(struct inode *inode)
{
	unsigned int flags = EXT4_I(inode)->i_flags;
	unsigned int new_fl = 0;

	if (flags & EXT4_SYNC_FL)
		new_fl |= S_SYNC;
	if (flags & EXT4_APPEND_FL)
		new_fl |= S_APPEND;
	if (flags & EXT4_IMMUTABLE_FL)
		new_fl |= S_IMMUTABLE;
	if (flags & EXT4_NOATIME_FL)
		new_fl |= S_NOATIME;
	if (flags & EXT4_DIRSYNC_FL)
		new_fl |= S_DIRSYNC;
	if (ext4_should_use_dax(inode))
		new_fl |= S_DAX;
	if (flags & EXT4_ENCRYPT_FL)
		new_fl |= S_ENCRYPTED;
	if (flags & EXT4_CASEFOLD_FL)
		new_fl |= S_CASEFOLD;
	if (flags & EXT4_VERITY_FL)
		new_fl |= S_VERITY;
	inode_set_flags(inode, new_fl,
			S_SYNC|S_APPEND|S_IMMUTABLE|S_NOATIME|S_DIRSYNC|S_DAX|
			S_ENCRYPTED|S_CASEFOLD|S_VERITY);
}
static unsigned int btrfs_inode_flags_to_xflags(unsigned int flags)
{
	unsigned int xflags = 0;

	if (flags & BTRFS_INODE_APPEND)
		xflags |= FS_XFLAG_APPEND;
	if (flags & BTRFS_INODE_IMMUTABLE)
		xflags |= FS_XFLAG_IMMUTABLE;
	if (flags & BTRFS_INODE_NOATIME)
		xflags |= FS_XFLAG_NOATIME;
	if (flags & BTRFS_INODE_NODUMP)
		xflags |= FS_XFLAG_NODUMP;
	if (flags & BTRFS_INODE_SYNC)
		xflags |= FS_XFLAG_SYNC;

	return xflags;
}
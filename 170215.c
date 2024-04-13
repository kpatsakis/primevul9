xfs_iunlock2_io_mmap(
	struct xfs_inode	*ip1,
	struct xfs_inode	*ip2)
{
	bool			same_inode = (ip1 == ip2);

	xfs_iunlock(ip2, XFS_MMAPLOCK_EXCL);
	if (!same_inode)
		xfs_iunlock(ip1, XFS_MMAPLOCK_EXCL);
	inode_unlock(VFS_I(ip2));
	if (!same_inode)
		inode_unlock(VFS_I(ip1));
}
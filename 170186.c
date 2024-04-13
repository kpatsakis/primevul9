xfs_ilock2_io_mmap(
	struct xfs_inode	*ip1,
	struct xfs_inode	*ip2)
{
	int			ret;

	ret = xfs_iolock_two_inodes_and_break_layout(VFS_I(ip1), VFS_I(ip2));
	if (ret)
		return ret;
	if (ip1 == ip2)
		xfs_ilock(ip1, XFS_MMAPLOCK_EXCL);
	else
		xfs_lock_two_inodes(ip1, XFS_MMAPLOCK_EXCL,
				    ip2, XFS_MMAPLOCK_EXCL);
	return 0;
}
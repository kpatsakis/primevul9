xfs_lock_inumorder(int lock_mode, int subclass)
{
	int	class = 0;

	ASSERT(!(lock_mode & (XFS_ILOCK_PARENT | XFS_ILOCK_RTBITMAP |
			      XFS_ILOCK_RTSUM)));
	ASSERT(xfs_lockdep_subclass_ok(subclass));

	if (lock_mode & (XFS_IOLOCK_SHARED|XFS_IOLOCK_EXCL)) {
		ASSERT(subclass <= XFS_IOLOCK_MAX_SUBCLASS);
		class += subclass << XFS_IOLOCK_SHIFT;
	}

	if (lock_mode & (XFS_MMAPLOCK_SHARED|XFS_MMAPLOCK_EXCL)) {
		ASSERT(subclass <= XFS_MMAPLOCK_MAX_SUBCLASS);
		class += subclass << XFS_MMAPLOCK_SHIFT;
	}

	if (lock_mode & (XFS_ILOCK_SHARED|XFS_ILOCK_EXCL)) {
		ASSERT(subclass <= XFS_ILOCK_MAX_SUBCLASS);
		class += subclass << XFS_ILOCK_SHIFT;
	}

	return (lock_mode & ~XFS_LOCK_SUBCLASS_MASK) | class;
}
xfs_isilocked(
	xfs_inode_t		*ip,
	uint			lock_flags)
{
	if (lock_flags & (XFS_ILOCK_EXCL|XFS_ILOCK_SHARED)) {
		if (!(lock_flags & XFS_ILOCK_SHARED))
			return !!ip->i_lock.mr_writer;
		return rwsem_is_locked(&ip->i_lock.mr_lock);
	}

	if (lock_flags & (XFS_MMAPLOCK_EXCL|XFS_MMAPLOCK_SHARED)) {
		if (!(lock_flags & XFS_MMAPLOCK_SHARED))
			return !!ip->i_mmaplock.mr_writer;
		return rwsem_is_locked(&ip->i_mmaplock.mr_lock);
	}

	if (lock_flags & (XFS_IOLOCK_EXCL|XFS_IOLOCK_SHARED)) {
		if (!(lock_flags & XFS_IOLOCK_SHARED))
			return !debug_locks ||
				lockdep_is_held_type(&VFS_I(ip)->i_rwsem, 0);
		return rwsem_is_locked(&VFS_I(ip)->i_rwsem);
	}

	ASSERT(0);
	return 0;
}
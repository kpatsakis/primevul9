xfs_fs_inode_init_once(
	void			*inode)
{
	struct xfs_inode	*ip = inode;

	memset(ip, 0, sizeof(struct xfs_inode));

	/* vfs inode */
	inode_init_once(VFS_I(ip));

	/* xfs inode */
	atomic_set(&ip->i_pincount, 0);
	spin_lock_init(&ip->i_flags_lock);

	mrlock_init(&ip->i_mmaplock, MRLOCK_ALLOW_EQUAL_PRI|MRLOCK_BARRIER,
		     "xfsino", ip->i_ino);
	mrlock_init(&ip->i_lock, MRLOCK_ALLOW_EQUAL_PRI|MRLOCK_BARRIER,
		     "xfsino", ip->i_ino);
}
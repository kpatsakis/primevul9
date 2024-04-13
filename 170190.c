xfs_finish_rename(
	struct xfs_trans	*tp)
{
	/*
	 * If this is a synchronous mount, make sure that the rename transaction
	 * goes to disk before returning to the user.
	 */
	if (tp->t_mountp->m_flags & (XFS_MOUNT_WSYNC|XFS_MOUNT_DIRSYNC))
		xfs_trans_set_sync(tp);

	return xfs_trans_commit(tp);
}
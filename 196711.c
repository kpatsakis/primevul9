xfs_fs_dirty_inode(
	struct inode			*inode,
	int				flag)
{
	struct xfs_inode		*ip = XFS_I(inode);
	struct xfs_mount		*mp = ip->i_mount;
	struct xfs_trans		*tp;

	if (!(inode->i_sb->s_flags & SB_LAZYTIME))
		return;
	if (flag != I_DIRTY_SYNC || !(inode->i_state & I_DIRTY_TIME))
		return;

	if (xfs_trans_alloc(mp, &M_RES(mp)->tr_fsyncts, 0, 0, 0, &tp))
		return;
	xfs_ilock(ip, XFS_ILOCK_EXCL);
	xfs_trans_ijoin(tp, ip, XFS_ILOCK_EXCL);
	xfs_trans_log_inode(tp, ip, XFS_ILOG_TIMESTAMP);
	xfs_trans_commit(tp);
}
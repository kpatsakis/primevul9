xfs_droplink(
	xfs_trans_t *tp,
	xfs_inode_t *ip)
{
	xfs_trans_ichgtime(tp, ip, XFS_ICHGTIME_CHG);

	drop_nlink(VFS_I(ip));
	xfs_trans_log_inode(tp, ip, XFS_ILOG_CORE);

	if (VFS_I(ip)->i_nlink)
		return 0;

	return xfs_iunlink(tp, ip);
}
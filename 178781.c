xfs_bmap_split_extent(
	struct xfs_inode        *ip,
	xfs_fileoff_t           split_fsb)
{
	struct xfs_mount        *mp = ip->i_mount;
	struct xfs_trans        *tp;
	struct xfs_defer_ops    dfops;
	xfs_fsblock_t           firstfsb;
	int                     error;

	error = xfs_trans_alloc(mp, &M_RES(mp)->tr_write,
			XFS_DIOSTRAT_SPACE_RES(mp, 0), 0, 0, &tp);
	if (error)
		return error;

	xfs_ilock(ip, XFS_ILOCK_EXCL);
	xfs_trans_ijoin(tp, ip, XFS_ILOCK_EXCL);

	xfs_defer_init(&dfops, &firstfsb);

	error = xfs_bmap_split_extent_at(tp, ip, split_fsb,
			&firstfsb, &dfops);
	if (error)
		goto out;

	error = xfs_defer_finish(&tp, &dfops);
	if (error)
		goto out;

	return xfs_trans_commit(tp);

out:
	xfs_defer_cancel(&dfops);
	xfs_trans_cancel(tp);
	return error;
}
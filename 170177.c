xfs_dir_ialloc(
	struct xfs_trans	**tpp,
	struct xfs_inode	*dp,
	umode_t			mode,
	xfs_nlink_t		nlink,
	dev_t			rdev,
	prid_t			prid,
	struct xfs_inode	**ipp)
{
	struct xfs_buf		*agibp;
	xfs_ino_t		parent_ino = dp ? dp->i_ino : 0;
	xfs_ino_t		ino;
	int			error;

	ASSERT((*tpp)->t_flags & XFS_TRANS_PERM_LOG_RES);

	/*
	 * Call the space management code to pick the on-disk inode to be
	 * allocated.
	 */
	error = xfs_dialloc_select_ag(tpp, parent_ino, mode, &agibp);
	if (error)
		return error;

	if (!agibp)
		return -ENOSPC;

	/* Allocate an inode from the selected AG */
	error = xfs_dialloc_ag(*tpp, agibp, parent_ino, &ino);
	if (error)
		return error;
	ASSERT(ino != NULLFSINO);

	return xfs_init_new_inode(*tpp, dp, ino, mode, nlink, rdev, prid, ipp);
}
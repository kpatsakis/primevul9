xfs_blkdev_get(
	xfs_mount_t		*mp,
	const char		*name,
	struct block_device	**bdevp)
{
	int			error = 0;

	*bdevp = blkdev_get_by_path(name, FMODE_READ|FMODE_WRITE|FMODE_EXCL,
				    mp);
	if (IS_ERR(*bdevp)) {
		error = PTR_ERR(*bdevp);
		xfs_warn(mp, "Invalid device [%s], error=%d", name, error);
	}

	return error;
}
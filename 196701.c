xfs_open_devices(
	struct xfs_mount	*mp)
{
	struct block_device	*ddev = mp->m_super->s_bdev;
	struct dax_device	*dax_ddev = fs_dax_get_by_bdev(ddev);
	struct dax_device	*dax_logdev = NULL, *dax_rtdev = NULL;
	struct block_device	*logdev = NULL, *rtdev = NULL;
	int			error;

	/*
	 * Open real time and log devices - order is important.
	 */
	if (mp->m_logname) {
		error = xfs_blkdev_get(mp, mp->m_logname, &logdev);
		if (error)
			goto out;
		dax_logdev = fs_dax_get_by_bdev(logdev);
	}

	if (mp->m_rtname) {
		error = xfs_blkdev_get(mp, mp->m_rtname, &rtdev);
		if (error)
			goto out_close_logdev;

		if (rtdev == ddev || rtdev == logdev) {
			xfs_warn(mp,
	"Cannot mount filesystem with identical rtdev and ddev/logdev.");
			error = -EINVAL;
			goto out_close_rtdev;
		}
		dax_rtdev = fs_dax_get_by_bdev(rtdev);
	}

	/*
	 * Setup xfs_mount buffer target pointers
	 */
	error = -ENOMEM;
	mp->m_ddev_targp = xfs_alloc_buftarg(mp, ddev, dax_ddev);
	if (!mp->m_ddev_targp)
		goto out_close_rtdev;

	if (rtdev) {
		mp->m_rtdev_targp = xfs_alloc_buftarg(mp, rtdev, dax_rtdev);
		if (!mp->m_rtdev_targp)
			goto out_free_ddev_targ;
	}

	if (logdev && logdev != ddev) {
		mp->m_logdev_targp = xfs_alloc_buftarg(mp, logdev, dax_logdev);
		if (!mp->m_logdev_targp)
			goto out_free_rtdev_targ;
	} else {
		mp->m_logdev_targp = mp->m_ddev_targp;
	}

	return 0;

 out_free_rtdev_targ:
	if (mp->m_rtdev_targp)
		xfs_free_buftarg(mp->m_rtdev_targp);
 out_free_ddev_targ:
	xfs_free_buftarg(mp->m_ddev_targp);
 out_close_rtdev:
	xfs_blkdev_put(rtdev);
	fs_put_dax(dax_rtdev);
 out_close_logdev:
	if (logdev && logdev != ddev) {
		xfs_blkdev_put(logdev);
		fs_put_dax(dax_logdev);
	}
 out:
	fs_put_dax(dax_ddev);
	return error;
}
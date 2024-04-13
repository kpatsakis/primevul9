xfs_close_devices(
	struct xfs_mount	*mp)
{
	struct dax_device *dax_ddev = mp->m_ddev_targp->bt_daxdev;

	if (mp->m_logdev_targp && mp->m_logdev_targp != mp->m_ddev_targp) {
		struct block_device *logdev = mp->m_logdev_targp->bt_bdev;
		struct dax_device *dax_logdev = mp->m_logdev_targp->bt_daxdev;

		xfs_free_buftarg(mp->m_logdev_targp);
		xfs_blkdev_put(logdev);
		fs_put_dax(dax_logdev);
	}
	if (mp->m_rtdev_targp) {
		struct block_device *rtdev = mp->m_rtdev_targp->bt_bdev;
		struct dax_device *dax_rtdev = mp->m_rtdev_targp->bt_daxdev;

		xfs_free_buftarg(mp->m_rtdev_targp);
		xfs_blkdev_put(rtdev);
		fs_put_dax(dax_rtdev);
	}
	xfs_free_buftarg(mp->m_ddev_targp);
	fs_put_dax(dax_ddev);
}
xfs_setup_devices(
	struct xfs_mount	*mp)
{
	int			error;

	error = xfs_setsize_buftarg(mp->m_ddev_targp, mp->m_sb.sb_sectsize);
	if (error)
		return error;

	if (mp->m_logdev_targp && mp->m_logdev_targp != mp->m_ddev_targp) {
		unsigned int	log_sector_size = BBSIZE;

		if (xfs_sb_version_hassector(&mp->m_sb))
			log_sector_size = mp->m_sb.sb_logsectsize;
		error = xfs_setsize_buftarg(mp->m_logdev_targp,
					    log_sector_size);
		if (error)
			return error;
	}
	if (mp->m_rtdev_targp) {
		error = xfs_setsize_buftarg(mp->m_rtdev_targp,
					    mp->m_sb.sb_sectsize);
		if (error)
			return error;
	}

	return 0;
}
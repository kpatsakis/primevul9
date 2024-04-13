xfs_finish_flags(
	struct xfs_mount	*mp)
{
	int			ronly = (mp->m_flags & XFS_MOUNT_RDONLY);

	/* Fail a mount where the logbuf is smaller than the log stripe */
	if (xfs_sb_version_haslogv2(&mp->m_sb)) {
		if (mp->m_logbsize <= 0 &&
		    mp->m_sb.sb_logsunit > XLOG_BIG_RECORD_BSIZE) {
			mp->m_logbsize = mp->m_sb.sb_logsunit;
		} else if (mp->m_logbsize > 0 &&
			   mp->m_logbsize < mp->m_sb.sb_logsunit) {
			xfs_warn(mp,
		"logbuf size must be greater than or equal to log stripe size");
			return -EINVAL;
		}
	} else {
		/* Fail a mount if the logbuf is larger than 32K */
		if (mp->m_logbsize > XLOG_BIG_RECORD_BSIZE) {
			xfs_warn(mp,
		"logbuf size for version 1 logs must be 16K or 32K");
			return -EINVAL;
		}
	}

	/*
	 * V5 filesystems always use attr2 format for attributes.
	 */
	if (xfs_sb_version_hascrc(&mp->m_sb) &&
	    (mp->m_flags & XFS_MOUNT_NOATTR2)) {
		xfs_warn(mp, "Cannot mount a V5 filesystem as noattr2. "
			     "attr2 is always enabled for V5 filesystems.");
		return -EINVAL;
	}

	/*
	 * mkfs'ed attr2 will turn on attr2 mount unless explicitly
	 * told by noattr2 to turn it off
	 */
	if (xfs_sb_version_hasattr2(&mp->m_sb) &&
	    !(mp->m_flags & XFS_MOUNT_NOATTR2))
		mp->m_flags |= XFS_MOUNT_ATTR2;

	/*
	 * prohibit r/w mounts of read-only filesystems
	 */
	if ((mp->m_sb.sb_flags & XFS_SBF_READONLY) && !ronly) {
		xfs_warn(mp,
			"cannot mount a read-only filesystem as read-write");
		return -EROFS;
	}

	if ((mp->m_qflags & (XFS_GQUOTA_ACCT | XFS_GQUOTA_ACTIVE)) &&
	    (mp->m_qflags & (XFS_PQUOTA_ACCT | XFS_PQUOTA_ACTIVE)) &&
	    !xfs_sb_version_has_pquotino(&mp->m_sb)) {
		xfs_warn(mp,
		  "Super block does not support project and group quota together");
		return -EINVAL;
	}

	return 0;
}
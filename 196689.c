xfs_showargs(
	struct xfs_mount	*mp,
	struct seq_file		*m)
{
	static struct proc_xfs_info xfs_info_set[] = {
		/* the few simple ones we can get from the mount struct */
		{ XFS_MOUNT_IKEEP,		",ikeep" },
		{ XFS_MOUNT_WSYNC,		",wsync" },
		{ XFS_MOUNT_NOALIGN,		",noalign" },
		{ XFS_MOUNT_SWALLOC,		",swalloc" },
		{ XFS_MOUNT_NOUUID,		",nouuid" },
		{ XFS_MOUNT_NORECOVERY,		",norecovery" },
		{ XFS_MOUNT_ATTR2,		",attr2" },
		{ XFS_MOUNT_FILESTREAMS,	",filestreams" },
		{ XFS_MOUNT_GRPID,		",grpid" },
		{ XFS_MOUNT_DISCARD,		",discard" },
		{ XFS_MOUNT_SMALL_INUMS,	",inode32" },
		{ XFS_MOUNT_DAX,		",dax" },
		{ 0, NULL }
	};
	static struct proc_xfs_info xfs_info_unset[] = {
		/* the few simple ones we can get from the mount struct */
		{ XFS_MOUNT_COMPAT_IOSIZE,	",largeio" },
		{ XFS_MOUNT_BARRIER,		",nobarrier" },
		{ XFS_MOUNT_SMALL_INUMS,	",inode64" },
		{ 0, NULL }
	};
	struct proc_xfs_info	*xfs_infop;

	for (xfs_infop = xfs_info_set; xfs_infop->flag; xfs_infop++) {
		if (mp->m_flags & xfs_infop->flag)
			seq_puts(m, xfs_infop->str);
	}
	for (xfs_infop = xfs_info_unset; xfs_infop->flag; xfs_infop++) {
		if (!(mp->m_flags & xfs_infop->flag))
			seq_puts(m, xfs_infop->str);
	}

	if (mp->m_flags & XFS_MOUNT_DFLT_IOSIZE)
		seq_printf(m, ",allocsize=%dk",
				(int)(1 << mp->m_writeio_log) >> 10);

	if (mp->m_logbufs > 0)
		seq_printf(m, ",logbufs=%d", mp->m_logbufs);
	if (mp->m_logbsize > 0)
		seq_printf(m, ",logbsize=%dk", mp->m_logbsize >> 10);

	if (mp->m_logname)
		seq_show_option(m, "logdev", mp->m_logname);
	if (mp->m_rtname)
		seq_show_option(m, "rtdev", mp->m_rtname);

	if (mp->m_dalign > 0)
		seq_printf(m, ",sunit=%d",
				(int)XFS_FSB_TO_BB(mp, mp->m_dalign));
	if (mp->m_swidth > 0)
		seq_printf(m, ",swidth=%d",
				(int)XFS_FSB_TO_BB(mp, mp->m_swidth));

	if (mp->m_qflags & (XFS_UQUOTA_ACCT|XFS_UQUOTA_ENFD))
		seq_puts(m, ",usrquota");
	else if (mp->m_qflags & XFS_UQUOTA_ACCT)
		seq_puts(m, ",uqnoenforce");

	if (mp->m_qflags & XFS_PQUOTA_ACCT) {
		if (mp->m_qflags & XFS_PQUOTA_ENFD)
			seq_puts(m, ",prjquota");
		else
			seq_puts(m, ",pqnoenforce");
	}
	if (mp->m_qflags & XFS_GQUOTA_ACCT) {
		if (mp->m_qflags & XFS_GQUOTA_ENFD)
			seq_puts(m, ",grpquota");
		else
			seq_puts(m, ",gqnoenforce");
	}

	if (!(mp->m_qflags & XFS_ALL_QUOTA_ACCT))
		seq_puts(m, ",noquota");

	return 0;
}
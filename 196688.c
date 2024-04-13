xfs_parseargs(
	struct xfs_mount	*mp,
	char			*options)
{
	const struct super_block *sb = mp->m_super;
	char			*p;
	substring_t		args[MAX_OPT_ARGS];
	int			dsunit = 0;
	int			dswidth = 0;
	int			iosize = 0;
	uint8_t			iosizelog = 0;

	/*
	 * set up the mount name first so all the errors will refer to the
	 * correct device.
	 */
	mp->m_fsname = kstrndup(sb->s_id, MAXNAMELEN, GFP_KERNEL);
	if (!mp->m_fsname)
		return -ENOMEM;
	mp->m_fsname_len = strlen(mp->m_fsname) + 1;

	/*
	 * Copy binary VFS mount flags we are interested in.
	 */
	if (sb_rdonly(sb))
		mp->m_flags |= XFS_MOUNT_RDONLY;
	if (sb->s_flags & SB_DIRSYNC)
		mp->m_flags |= XFS_MOUNT_DIRSYNC;
	if (sb->s_flags & SB_SYNCHRONOUS)
		mp->m_flags |= XFS_MOUNT_WSYNC;

	/*
	 * Set some default flags that could be cleared by the mount option
	 * parsing.
	 */
	mp->m_flags |= XFS_MOUNT_BARRIER;
	mp->m_flags |= XFS_MOUNT_COMPAT_IOSIZE;

	/*
	 * These can be overridden by the mount option parsing.
	 */
	mp->m_logbufs = -1;
	mp->m_logbsize = -1;

	if (!options)
		goto done;

	while ((p = strsep(&options, ",")) != NULL) {
		int		token;

		if (!*p)
			continue;

		token = match_token(p, tokens, args);
		switch (token) {
		case Opt_logbufs:
			if (match_int(args, &mp->m_logbufs))
				return -EINVAL;
			break;
		case Opt_logbsize:
			if (suffix_kstrtoint(args, 10, &mp->m_logbsize))
				return -EINVAL;
			break;
		case Opt_logdev:
			kfree(mp->m_logname);
			mp->m_logname = match_strdup(args);
			if (!mp->m_logname)
				return -ENOMEM;
			break;
		case Opt_mtpt:
			xfs_warn(mp, "%s option not allowed on this system", p);
			return -EINVAL;
		case Opt_rtdev:
			kfree(mp->m_rtname);
			mp->m_rtname = match_strdup(args);
			if (!mp->m_rtname)
				return -ENOMEM;
			break;
		case Opt_allocsize:
		case Opt_biosize:
			if (suffix_kstrtoint(args, 10, &iosize))
				return -EINVAL;
			iosizelog = ffs(iosize) - 1;
			break;
		case Opt_grpid:
		case Opt_bsdgroups:
			mp->m_flags |= XFS_MOUNT_GRPID;
			break;
		case Opt_nogrpid:
		case Opt_sysvgroups:
			mp->m_flags &= ~XFS_MOUNT_GRPID;
			break;
		case Opt_wsync:
			mp->m_flags |= XFS_MOUNT_WSYNC;
			break;
		case Opt_norecovery:
			mp->m_flags |= XFS_MOUNT_NORECOVERY;
			break;
		case Opt_noalign:
			mp->m_flags |= XFS_MOUNT_NOALIGN;
			break;
		case Opt_swalloc:
			mp->m_flags |= XFS_MOUNT_SWALLOC;
			break;
		case Opt_sunit:
			if (match_int(args, &dsunit))
				return -EINVAL;
			break;
		case Opt_swidth:
			if (match_int(args, &dswidth))
				return -EINVAL;
			break;
		case Opt_inode32:
			mp->m_flags |= XFS_MOUNT_SMALL_INUMS;
			break;
		case Opt_inode64:
			mp->m_flags &= ~XFS_MOUNT_SMALL_INUMS;
			break;
		case Opt_nouuid:
			mp->m_flags |= XFS_MOUNT_NOUUID;
			break;
		case Opt_ikeep:
			mp->m_flags |= XFS_MOUNT_IKEEP;
			break;
		case Opt_noikeep:
			mp->m_flags &= ~XFS_MOUNT_IKEEP;
			break;
		case Opt_largeio:
			mp->m_flags &= ~XFS_MOUNT_COMPAT_IOSIZE;
			break;
		case Opt_nolargeio:
			mp->m_flags |= XFS_MOUNT_COMPAT_IOSIZE;
			break;
		case Opt_attr2:
			mp->m_flags |= XFS_MOUNT_ATTR2;
			break;
		case Opt_noattr2:
			mp->m_flags &= ~XFS_MOUNT_ATTR2;
			mp->m_flags |= XFS_MOUNT_NOATTR2;
			break;
		case Opt_filestreams:
			mp->m_flags |= XFS_MOUNT_FILESTREAMS;
			break;
		case Opt_noquota:
			mp->m_qflags &= ~XFS_ALL_QUOTA_ACCT;
			mp->m_qflags &= ~XFS_ALL_QUOTA_ENFD;
			mp->m_qflags &= ~XFS_ALL_QUOTA_ACTIVE;
			break;
		case Opt_quota:
		case Opt_uquota:
		case Opt_usrquota:
			mp->m_qflags |= (XFS_UQUOTA_ACCT | XFS_UQUOTA_ACTIVE |
					 XFS_UQUOTA_ENFD);
			break;
		case Opt_qnoenforce:
		case Opt_uqnoenforce:
			mp->m_qflags |= (XFS_UQUOTA_ACCT | XFS_UQUOTA_ACTIVE);
			mp->m_qflags &= ~XFS_UQUOTA_ENFD;
			break;
		case Opt_pquota:
		case Opt_prjquota:
			mp->m_qflags |= (XFS_PQUOTA_ACCT | XFS_PQUOTA_ACTIVE |
					 XFS_PQUOTA_ENFD);
			break;
		case Opt_pqnoenforce:
			mp->m_qflags |= (XFS_PQUOTA_ACCT | XFS_PQUOTA_ACTIVE);
			mp->m_qflags &= ~XFS_PQUOTA_ENFD;
			break;
		case Opt_gquota:
		case Opt_grpquota:
			mp->m_qflags |= (XFS_GQUOTA_ACCT | XFS_GQUOTA_ACTIVE |
					 XFS_GQUOTA_ENFD);
			break;
		case Opt_gqnoenforce:
			mp->m_qflags |= (XFS_GQUOTA_ACCT | XFS_GQUOTA_ACTIVE);
			mp->m_qflags &= ~XFS_GQUOTA_ENFD;
			break;
		case Opt_discard:
			mp->m_flags |= XFS_MOUNT_DISCARD;
			break;
		case Opt_nodiscard:
			mp->m_flags &= ~XFS_MOUNT_DISCARD;
			break;
#ifdef CONFIG_FS_DAX
		case Opt_dax:
			mp->m_flags |= XFS_MOUNT_DAX;
			break;
#endif
		case Opt_barrier:
			xfs_warn(mp, "%s option is deprecated, ignoring.", p);
			mp->m_flags |= XFS_MOUNT_BARRIER;
			break;
		case Opt_nobarrier:
			xfs_warn(mp, "%s option is deprecated, ignoring.", p);
			mp->m_flags &= ~XFS_MOUNT_BARRIER;
			break;
		default:
			xfs_warn(mp, "unknown mount option [%s].", p);
			return -EINVAL;
		}
	}

	/*
	 * no recovery flag requires a read-only mount
	 */
	if ((mp->m_flags & XFS_MOUNT_NORECOVERY) &&
	    !(mp->m_flags & XFS_MOUNT_RDONLY)) {
		xfs_warn(mp, "no-recovery mounts must be read-only.");
		return -EINVAL;
	}

	if ((mp->m_flags & XFS_MOUNT_NOALIGN) && (dsunit || dswidth)) {
		xfs_warn(mp,
	"sunit and swidth options incompatible with the noalign option");
		return -EINVAL;
	}

#ifndef CONFIG_XFS_QUOTA
	if (XFS_IS_QUOTA_RUNNING(mp)) {
		xfs_warn(mp, "quota support not available in this kernel.");
		return -EINVAL;
	}
#endif

	if ((dsunit && !dswidth) || (!dsunit && dswidth)) {
		xfs_warn(mp, "sunit and swidth must be specified together");
		return -EINVAL;
	}

	if (dsunit && (dswidth % dsunit != 0)) {
		xfs_warn(mp,
	"stripe width (%d) must be a multiple of the stripe unit (%d)",
			dswidth, dsunit);
		return -EINVAL;
	}

done:
	if (dsunit && !(mp->m_flags & XFS_MOUNT_NOALIGN)) {
		/*
		 * At this point the superblock has not been read
		 * in, therefore we do not know the block size.
		 * Before the mount call ends we will convert
		 * these to FSBs.
		 */
		mp->m_dalign = dsunit;
		mp->m_swidth = dswidth;
	}

	if (mp->m_logbufs != -1 &&
	    mp->m_logbufs != 0 &&
	    (mp->m_logbufs < XLOG_MIN_ICLOGS ||
	     mp->m_logbufs > XLOG_MAX_ICLOGS)) {
		xfs_warn(mp, "invalid logbufs value: %d [not %d-%d]",
			mp->m_logbufs, XLOG_MIN_ICLOGS, XLOG_MAX_ICLOGS);
		return -EINVAL;
	}
	if (mp->m_logbsize != -1 &&
	    mp->m_logbsize !=  0 &&
	    (mp->m_logbsize < XLOG_MIN_RECORD_BSIZE ||
	     mp->m_logbsize > XLOG_MAX_RECORD_BSIZE ||
	     !is_power_of_2(mp->m_logbsize))) {
		xfs_warn(mp,
			"invalid logbufsize: %d [not 16k,32k,64k,128k or 256k]",
			mp->m_logbsize);
		return -EINVAL;
	}

	if (iosizelog) {
		if (iosizelog > XFS_MAX_IO_LOG ||
		    iosizelog < XFS_MIN_IO_LOG) {
			xfs_warn(mp, "invalid log iosize: %d [not %d-%d]",
				iosizelog, XFS_MIN_IO_LOG,
				XFS_MAX_IO_LOG);
			return -EINVAL;
		}

		mp->m_flags |= XFS_MOUNT_DFLT_IOSIZE;
		mp->m_readio_log = iosizelog;
		mp->m_writeio_log = iosizelog;
	}

	return 0;
}
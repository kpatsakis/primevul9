xfs_fs_remount(
	struct super_block	*sb,
	int			*flags,
	char			*options)
{
	struct xfs_mount	*mp = XFS_M(sb);
	xfs_sb_t		*sbp = &mp->m_sb;
	substring_t		args[MAX_OPT_ARGS];
	char			*p;
	int			error;

	/* First, check for complete junk; i.e. invalid options */
	error = xfs_test_remount_options(sb, options);
	if (error)
		return error;

	sync_filesystem(sb);
	while ((p = strsep(&options, ",")) != NULL) {
		int token;

		if (!*p)
			continue;

		token = match_token(p, tokens, args);
		switch (token) {
		case Opt_barrier:
			xfs_warn(mp, "%s option is deprecated, ignoring.", p);
			mp->m_flags |= XFS_MOUNT_BARRIER;
			break;
		case Opt_nobarrier:
			xfs_warn(mp, "%s option is deprecated, ignoring.", p);
			mp->m_flags &= ~XFS_MOUNT_BARRIER;
			break;
		case Opt_inode64:
			mp->m_flags &= ~XFS_MOUNT_SMALL_INUMS;
			mp->m_maxagi = xfs_set_inode_alloc(mp, sbp->sb_agcount);
			break;
		case Opt_inode32:
			mp->m_flags |= XFS_MOUNT_SMALL_INUMS;
			mp->m_maxagi = xfs_set_inode_alloc(mp, sbp->sb_agcount);
			break;
		default:
			/*
			 * Logically we would return an error here to prevent
			 * users from believing they might have changed
			 * mount options using remount which can't be changed.
			 *
			 * But unfortunately mount(8) adds all options from
			 * mtab and fstab to the mount arguments in some cases
			 * so we can't blindly reject options, but have to
			 * check for each specified option if it actually
			 * differs from the currently set option and only
			 * reject it if that's the case.
			 *
			 * Until that is implemented we return success for
			 * every remount request, and silently ignore all
			 * options that we can't actually change.
			 */
#if 0
			xfs_info(mp,
		"mount option \"%s\" not supported for remount", p);
			return -EINVAL;
#else
			break;
#endif
		}
	}

	/* ro -> rw */
	if ((mp->m_flags & XFS_MOUNT_RDONLY) && !(*flags & SB_RDONLY)) {
		if (mp->m_flags & XFS_MOUNT_NORECOVERY) {
			xfs_warn(mp,
		"ro->rw transition prohibited on norecovery mount");
			return -EINVAL;
		}

		if (XFS_SB_VERSION_NUM(sbp) == XFS_SB_VERSION_5 &&
		    xfs_sb_has_ro_compat_feature(sbp,
					XFS_SB_FEAT_RO_COMPAT_UNKNOWN)) {
			xfs_warn(mp,
"ro->rw transition prohibited on unknown (0x%x) ro-compat filesystem",
				(sbp->sb_features_ro_compat &
					XFS_SB_FEAT_RO_COMPAT_UNKNOWN));
			return -EINVAL;
		}

		mp->m_flags &= ~XFS_MOUNT_RDONLY;

		/*
		 * If this is the first remount to writeable state we
		 * might have some superblock changes to update.
		 */
		if (mp->m_update_sb) {
			error = xfs_sync_sb(mp, false);
			if (error) {
				xfs_warn(mp, "failed to write sb changes");
				return error;
			}
			mp->m_update_sb = false;
		}

		/*
		 * Fill out the reserve pool if it is empty. Use the stashed
		 * value if it is non-zero, otherwise go with the default.
		 */
		xfs_restore_resvblks(mp);
		xfs_log_work_queue(mp);

		/* Recover any CoW blocks that never got remapped. */
		error = xfs_reflink_recover_cow(mp);
		if (error) {
			xfs_err(mp,
	"Error %d recovering leftover CoW allocations.", error);
			xfs_force_shutdown(mp, SHUTDOWN_CORRUPT_INCORE);
			return error;
		}
		xfs_icache_enable_reclaim(mp);

		/* Create the per-AG metadata reservation pool .*/
		error = xfs_fs_reserve_ag_blocks(mp);
		if (error && error != -ENOSPC)
			return error;
	}

	/* rw -> ro */
	if (!(mp->m_flags & XFS_MOUNT_RDONLY) && (*flags & SB_RDONLY)) {
		/*
		 * Cancel background eofb scanning so it cannot race with the
		 * final log force+buftarg wait and deadlock the remount.
		 */
		xfs_icache_disable_reclaim(mp);

		/* Get rid of any leftover CoW reservations... */
		error = xfs_icache_free_cowblocks(mp, NULL);
		if (error) {
			xfs_force_shutdown(mp, SHUTDOWN_CORRUPT_INCORE);
			return error;
		}

		/* Free the per-AG metadata reservation pool. */
		error = xfs_fs_unreserve_ag_blocks(mp);
		if (error) {
			xfs_force_shutdown(mp, SHUTDOWN_CORRUPT_INCORE);
			return error;
		}

		/*
		 * Before we sync the metadata, we need to free up the reserve
		 * block pool so that the used block count in the superblock on
		 * disk is correct at the end of the remount. Stash the current
		 * reserve pool size so that if we get remounted rw, we can
		 * return it to the same size.
		 */
		xfs_save_resvblks(mp);

		xfs_quiesce_attr(mp);
		mp->m_flags |= XFS_MOUNT_RDONLY;
	}

	return 0;
}
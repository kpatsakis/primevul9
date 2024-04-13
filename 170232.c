xfs_create(
	xfs_inode_t		*dp,
	struct xfs_name		*name,
	umode_t			mode,
	dev_t			rdev,
	xfs_inode_t		**ipp)
{
	int			is_dir = S_ISDIR(mode);
	struct xfs_mount	*mp = dp->i_mount;
	struct xfs_inode	*ip = NULL;
	struct xfs_trans	*tp = NULL;
	int			error;
	bool                    unlock_dp_on_error = false;
	prid_t			prid;
	struct xfs_dquot	*udqp = NULL;
	struct xfs_dquot	*gdqp = NULL;
	struct xfs_dquot	*pdqp = NULL;
	struct xfs_trans_res	*tres;
	uint			resblks;

	trace_xfs_create(dp, name);

	if (XFS_FORCED_SHUTDOWN(mp))
		return -EIO;

	prid = xfs_get_initial_prid(dp);

	/*
	 * Make sure that we have allocated dquot(s) on disk.
	 */
	error = xfs_qm_vop_dqalloc(dp, current_fsuid(), current_fsgid(), prid,
					XFS_QMOPT_QUOTALL | XFS_QMOPT_INHERIT,
					&udqp, &gdqp, &pdqp);
	if (error)
		return error;

	if (is_dir) {
		resblks = XFS_MKDIR_SPACE_RES(mp, name->len);
		tres = &M_RES(mp)->tr_mkdir;
	} else {
		resblks = XFS_CREATE_SPACE_RES(mp, name->len);
		tres = &M_RES(mp)->tr_create;
	}

	/*
	 * Initially assume that the file does not exist and
	 * reserve the resources for that case.  If that is not
	 * the case we'll drop the one we have and get a more
	 * appropriate transaction later.
	 */
	error = xfs_trans_alloc(mp, tres, resblks, 0, 0, &tp);
	if (error == -ENOSPC) {
		/* flush outstanding delalloc blocks and retry */
		xfs_flush_inodes(mp);
		error = xfs_trans_alloc(mp, tres, resblks, 0, 0, &tp);
	}
	if (error)
		goto out_release_inode;

	xfs_ilock(dp, XFS_ILOCK_EXCL | XFS_ILOCK_PARENT);
	unlock_dp_on_error = true;

	/*
	 * Reserve disk quota and the inode.
	 */
	error = xfs_trans_reserve_quota(tp, mp, udqp, gdqp,
						pdqp, resblks, 1, 0);
	if (error)
		goto out_trans_cancel;

	error = xfs_iext_count_may_overflow(dp, XFS_DATA_FORK,
			XFS_IEXT_DIR_MANIP_CNT(mp));
	if (error)
		goto out_trans_cancel;

	/*
	 * A newly created regular or special file just has one directory
	 * entry pointing to them, but a directory also the "." entry
	 * pointing to itself.
	 */
	error = xfs_dir_ialloc(&tp, dp, mode, is_dir ? 2 : 1, rdev, prid, &ip);
	if (error)
		goto out_trans_cancel;

	/*
	 * Now we join the directory inode to the transaction.  We do not do it
	 * earlier because xfs_dir_ialloc might commit the previous transaction
	 * (and release all the locks).  An error from here on will result in
	 * the transaction cancel unlocking dp so don't do it explicitly in the
	 * error path.
	 */
	xfs_trans_ijoin(tp, dp, XFS_ILOCK_EXCL);
	unlock_dp_on_error = false;

	error = xfs_dir_createname(tp, dp, name, ip->i_ino,
					resblks - XFS_IALLOC_SPACE_RES(mp));
	if (error) {
		ASSERT(error != -ENOSPC);
		goto out_trans_cancel;
	}
	xfs_trans_ichgtime(tp, dp, XFS_ICHGTIME_MOD | XFS_ICHGTIME_CHG);
	xfs_trans_log_inode(tp, dp, XFS_ILOG_CORE);

	if (is_dir) {
		error = xfs_dir_init(tp, ip, dp);
		if (error)
			goto out_trans_cancel;

		xfs_bumplink(tp, dp);
	}

	/*
	 * If this is a synchronous mount, make sure that the
	 * create transaction goes to disk before returning to
	 * the user.
	 */
	if (mp->m_flags & (XFS_MOUNT_WSYNC|XFS_MOUNT_DIRSYNC))
		xfs_trans_set_sync(tp);

	/*
	 * Attach the dquot(s) to the inodes and modify them incore.
	 * These ids of the inode couldn't have changed since the new
	 * inode has been locked ever since it was created.
	 */
	xfs_qm_vop_create_dqattach(tp, ip, udqp, gdqp, pdqp);

	error = xfs_trans_commit(tp);
	if (error)
		goto out_release_inode;

	xfs_qm_dqrele(udqp);
	xfs_qm_dqrele(gdqp);
	xfs_qm_dqrele(pdqp);

	*ipp = ip;
	return 0;

 out_trans_cancel:
	xfs_trans_cancel(tp);
 out_release_inode:
	/*
	 * Wait until after the current transaction is aborted to finish the
	 * setup of the inode and release the inode.  This prevents recursive
	 * transactions and deadlocks from xfs_inactive.
	 */
	if (ip) {
		xfs_finish_inode_setup(ip);
		xfs_irele(ip);
	}

	xfs_qm_dqrele(udqp);
	xfs_qm_dqrele(gdqp);
	xfs_qm_dqrele(pdqp);

	if (unlock_dp_on_error)
		xfs_iunlock(dp, XFS_ILOCK_EXCL);
	return error;
}
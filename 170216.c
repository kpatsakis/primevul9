xfs_create_tmpfile(
	struct xfs_inode	*dp,
	umode_t			mode,
	struct xfs_inode	**ipp)
{
	struct xfs_mount	*mp = dp->i_mount;
	struct xfs_inode	*ip = NULL;
	struct xfs_trans	*tp = NULL;
	int			error;
	prid_t                  prid;
	struct xfs_dquot	*udqp = NULL;
	struct xfs_dquot	*gdqp = NULL;
	struct xfs_dquot	*pdqp = NULL;
	struct xfs_trans_res	*tres;
	uint			resblks;

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

	resblks = XFS_IALLOC_SPACE_RES(mp);
	tres = &M_RES(mp)->tr_create_tmpfile;

	error = xfs_trans_alloc(mp, tres, resblks, 0, 0, &tp);
	if (error)
		goto out_release_inode;

	error = xfs_trans_reserve_quota(tp, mp, udqp, gdqp,
						pdqp, resblks, 1, 0);
	if (error)
		goto out_trans_cancel;

	error = xfs_dir_ialloc(&tp, dp, mode, 0, 0, prid, &ip);
	if (error)
		goto out_trans_cancel;

	if (mp->m_flags & XFS_MOUNT_WSYNC)
		xfs_trans_set_sync(tp);

	/*
	 * Attach the dquot(s) to the inodes and modify them incore.
	 * These ids of the inode couldn't have changed since the new
	 * inode has been locked ever since it was created.
	 */
	xfs_qm_vop_create_dqattach(tp, ip, udqp, gdqp, pdqp);

	error = xfs_iunlink(tp, ip);
	if (error)
		goto out_trans_cancel;

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

	return error;
}
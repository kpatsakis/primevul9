xfs_fs_statfs(
	struct dentry		*dentry,
	struct kstatfs		*statp)
{
	struct xfs_mount	*mp = XFS_M(dentry->d_sb);
	xfs_sb_t		*sbp = &mp->m_sb;
	struct xfs_inode	*ip = XFS_I(d_inode(dentry));
	uint64_t		fakeinos, id;
	uint64_t		icount;
	uint64_t		ifree;
	uint64_t		fdblocks;
	xfs_extlen_t		lsize;
	int64_t			ffree;

	statp->f_type = XFS_SB_MAGIC;
	statp->f_namelen = MAXNAMELEN - 1;

	id = huge_encode_dev(mp->m_ddev_targp->bt_dev);
	statp->f_fsid.val[0] = (u32)id;
	statp->f_fsid.val[1] = (u32)(id >> 32);

	icount = percpu_counter_sum(&mp->m_icount);
	ifree = percpu_counter_sum(&mp->m_ifree);
	fdblocks = percpu_counter_sum(&mp->m_fdblocks);

	spin_lock(&mp->m_sb_lock);
	statp->f_bsize = sbp->sb_blocksize;
	lsize = sbp->sb_logstart ? sbp->sb_logblocks : 0;
	statp->f_blocks = sbp->sb_dblocks - lsize;
	spin_unlock(&mp->m_sb_lock);

	statp->f_bfree = fdblocks - mp->m_alloc_set_aside;
	statp->f_bavail = statp->f_bfree;

	fakeinos = statp->f_bfree << sbp->sb_inopblog;
	statp->f_files = MIN(icount + fakeinos, (uint64_t)XFS_MAXINUMBER);
	if (mp->m_maxicount)
		statp->f_files = min_t(typeof(statp->f_files),
					statp->f_files,
					mp->m_maxicount);

	/* If sb_icount overshot maxicount, report actual allocation */
	statp->f_files = max_t(typeof(statp->f_files),
					statp->f_files,
					sbp->sb_icount);

	/* make sure statp->f_ffree does not underflow */
	ffree = statp->f_files - (icount - ifree);
	statp->f_ffree = max_t(int64_t, ffree, 0);


	if ((ip->i_d.di_flags & XFS_DIFLAG_PROJINHERIT) &&
	    ((mp->m_qflags & (XFS_PQUOTA_ACCT|XFS_PQUOTA_ENFD))) ==
			      (XFS_PQUOTA_ACCT|XFS_PQUOTA_ENFD))
		xfs_qm_statvfs(ip, statp);

	if (XFS_IS_REALTIME_MOUNT(mp) &&
	    (ip->i_d.di_flags & (XFS_DIFLAG_RTINHERIT | XFS_DIFLAG_REALTIME))) {
		statp->f_blocks = sbp->sb_rblocks;
		statp->f_bavail = statp->f_bfree =
			sbp->sb_frextents * sbp->sb_rextsize;
	}

	return 0;
}
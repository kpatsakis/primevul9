xfs_ifree(
	struct xfs_trans	*tp,
	struct xfs_inode	*ip)
{
	int			error;
	struct xfs_icluster	xic = { 0 };
	struct xfs_inode_log_item *iip = ip->i_itemp;

	ASSERT(xfs_isilocked(ip, XFS_ILOCK_EXCL));
	ASSERT(VFS_I(ip)->i_nlink == 0);
	ASSERT(ip->i_df.if_nextents == 0);
	ASSERT(ip->i_d.di_size == 0 || !S_ISREG(VFS_I(ip)->i_mode));
	ASSERT(ip->i_d.di_nblocks == 0);

	/*
	 * Pull the on-disk inode from the AGI unlinked list.
	 */
	error = xfs_iunlink_remove(tp, ip);
	if (error)
		return error;

	error = xfs_difree(tp, ip->i_ino, &xic);
	if (error)
		return error;

	/*
	 * Free any local-format data sitting around before we reset the
	 * data fork to extents format.  Note that the attr fork data has
	 * already been freed by xfs_attr_inactive.
	 */
	if (ip->i_df.if_format == XFS_DINODE_FMT_LOCAL) {
		kmem_free(ip->i_df.if_u1.if_data);
		ip->i_df.if_u1.if_data = NULL;
		ip->i_df.if_bytes = 0;
	}

	VFS_I(ip)->i_mode = 0;		/* mark incore inode as free */
	ip->i_d.di_flags = 0;
	ip->i_d.di_flags2 = ip->i_mount->m_ino_geo.new_diflags2;
	ip->i_d.di_dmevmask = 0;
	ip->i_d.di_forkoff = 0;		/* mark the attr fork not in use */
	ip->i_df.if_format = XFS_DINODE_FMT_EXTENTS;

	/* Don't attempt to replay owner changes for a deleted inode */
	spin_lock(&iip->ili_lock);
	iip->ili_fields &= ~(XFS_ILOG_AOWNER | XFS_ILOG_DOWNER);
	spin_unlock(&iip->ili_lock);

	/*
	 * Bump the generation count so no one will be confused
	 * by reincarnations of this inode.
	 */
	VFS_I(ip)->i_generation++;
	xfs_trans_log_inode(tp, ip, XFS_ILOG_CORE);

	if (xic.deleted)
		error = xfs_ifree_cluster(ip, tp, &xic);

	return error;
}
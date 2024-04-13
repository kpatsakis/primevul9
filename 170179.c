xfs_ifree_cluster(
	struct xfs_inode	*free_ip,
	struct xfs_trans	*tp,
	struct xfs_icluster	*xic)
{
	struct xfs_mount	*mp = free_ip->i_mount;
	struct xfs_ino_geometry	*igeo = M_IGEO(mp);
	struct xfs_buf		*bp;
	xfs_daddr_t		blkno;
	xfs_ino_t		inum = xic->first_ino;
	int			nbufs;
	int			i, j;
	int			ioffset;
	int			error;

	nbufs = igeo->ialloc_blks / igeo->blocks_per_cluster;

	for (j = 0; j < nbufs; j++, inum += igeo->inodes_per_cluster) {
		/*
		 * The allocation bitmap tells us which inodes of the chunk were
		 * physically allocated. Skip the cluster if an inode falls into
		 * a sparse region.
		 */
		ioffset = inum - xic->first_ino;
		if ((xic->alloc & XFS_INOBT_MASK(ioffset)) == 0) {
			ASSERT(ioffset % igeo->inodes_per_cluster == 0);
			continue;
		}

		blkno = XFS_AGB_TO_DADDR(mp, XFS_INO_TO_AGNO(mp, inum),
					 XFS_INO_TO_AGBNO(mp, inum));

		/*
		 * We obtain and lock the backing buffer first in the process
		 * here to ensure dirty inodes attached to the buffer remain in
		 * the flushing state while we mark them stale.
		 *
		 * If we scan the in-memory inodes first, then buffer IO can
		 * complete before we get a lock on it, and hence we may fail
		 * to mark all the active inodes on the buffer stale.
		 */
		error = xfs_trans_get_buf(tp, mp->m_ddev_targp, blkno,
				mp->m_bsize * igeo->blocks_per_cluster,
				XBF_UNMAPPED, &bp);
		if (error)
			return error;

		/*
		 * This buffer may not have been correctly initialised as we
		 * didn't read it from disk. That's not important because we are
		 * only using to mark the buffer as stale in the log, and to
		 * attach stale cached inodes on it. That means it will never be
		 * dispatched for IO. If it is, we want to know about it, and we
		 * want it to fail. We can acheive this by adding a write
		 * verifier to the buffer.
		 */
		bp->b_ops = &xfs_inode_buf_ops;

		/*
		 * Now we need to set all the cached clean inodes as XFS_ISTALE,
		 * too. This requires lookups, and will skip inodes that we've
		 * already marked XFS_ISTALE.
		 */
		for (i = 0; i < igeo->inodes_per_cluster; i++)
			xfs_ifree_mark_inode_stale(bp, free_ip, inum + i);

		xfs_trans_stale_inode_buf(tp, bp);
		xfs_trans_binval(tp, bp);
	}
	return 0;
}
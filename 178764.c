xfs_bmap_worst_indlen(
	xfs_inode_t	*ip,		/* incore inode pointer */
	xfs_filblks_t	len)		/* delayed extent length */
{
	int		level;		/* btree level number */
	int		maxrecs;	/* maximum record count at this level */
	xfs_mount_t	*mp;		/* mount structure */
	xfs_filblks_t	rval;		/* return value */

	mp = ip->i_mount;
	maxrecs = mp->m_bmap_dmxr[0];
	for (level = 0, rval = 0;
	     level < XFS_BM_MAXLEVELS(mp, XFS_DATA_FORK);
	     level++) {
		len += maxrecs - 1;
		do_div(len, maxrecs);
		rval += len;
		if (len == 1)
			return rval + XFS_BM_MAXLEVELS(mp, XFS_DATA_FORK) -
				level - 1;
		if (level == 0)
			maxrecs = mp->m_bmap_dmxr[1];
	}
	return rval;
}
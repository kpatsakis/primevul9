xfs_bmap_one_block(
	xfs_inode_t	*ip,		/* incore inode */
	int		whichfork)	/* data or attr fork */
{
	xfs_ifork_t	*ifp;		/* inode fork pointer */
	int		rval;		/* return value */
	xfs_bmbt_irec_t	s;		/* internal version of extent */
	struct xfs_iext_cursor icur;

#ifndef DEBUG
	if (whichfork == XFS_DATA_FORK)
		return XFS_ISIZE(ip) == ip->i_mount->m_sb.sb_blocksize;
#endif	/* !DEBUG */
	if (XFS_IFORK_NEXTENTS(ip, whichfork) != 1)
		return 0;
	if (XFS_IFORK_FORMAT(ip, whichfork) != XFS_DINODE_FMT_EXTENTS)
		return 0;
	ifp = XFS_IFORK_PTR(ip, whichfork);
	ASSERT(ifp->if_flags & XFS_IFEXTENTS);
	xfs_iext_first(ifp, &icur);
	xfs_iext_get_extent(ifp, &icur, &s);
	rval = s.br_startoff == 0 && s.br_blockcount == 1;
	if (rval && whichfork == XFS_DATA_FORK)
		ASSERT(XFS_ISIZE(ip) == ip->i_mount->m_sb.sb_blocksize);
	return rval;
}
xfs_bmap_first_unused(
	struct xfs_trans	*tp,		/* transaction pointer */
	struct xfs_inode	*ip,		/* incore inode */
	xfs_extlen_t		len,		/* size of hole to find */
	xfs_fileoff_t		*first_unused,	/* unused block */
	int			whichfork)	/* data or attr fork */
{
	struct xfs_ifork	*ifp = XFS_IFORK_PTR(ip, whichfork);
	struct xfs_bmbt_irec	got;
	struct xfs_iext_cursor	icur;
	xfs_fileoff_t		lastaddr = 0;
	xfs_fileoff_t		lowest, max;
	int			error;

	ASSERT(XFS_IFORK_FORMAT(ip, whichfork) == XFS_DINODE_FMT_BTREE ||
	       XFS_IFORK_FORMAT(ip, whichfork) == XFS_DINODE_FMT_EXTENTS ||
	       XFS_IFORK_FORMAT(ip, whichfork) == XFS_DINODE_FMT_LOCAL);

	if (XFS_IFORK_FORMAT(ip, whichfork) == XFS_DINODE_FMT_LOCAL) {
		*first_unused = 0;
		return 0;
	}

	if (!(ifp->if_flags & XFS_IFEXTENTS)) {
		error = xfs_iread_extents(tp, ip, whichfork);
		if (error)
			return error;
	}

	lowest = max = *first_unused;
	for_each_xfs_iext(ifp, &icur, &got) {
		/*
		 * See if the hole before this extent will work.
		 */
		if (got.br_startoff >= lowest + len &&
		    got.br_startoff - max >= len)
			break;
		lastaddr = got.br_startoff + got.br_blockcount;
		max = XFS_FILEOFF_MAX(lastaddr, lowest);
	}

	*first_unused = max;
	return 0;
}
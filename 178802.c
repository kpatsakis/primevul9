xfs_bmap_last_extent(
	struct xfs_trans	*tp,
	struct xfs_inode	*ip,
	int			whichfork,
	struct xfs_bmbt_irec	*rec,
	int			*is_empty)
{
	struct xfs_ifork	*ifp = XFS_IFORK_PTR(ip, whichfork);
	struct xfs_iext_cursor	icur;
	int			error;

	if (!(ifp->if_flags & XFS_IFEXTENTS)) {
		error = xfs_iread_extents(tp, ip, whichfork);
		if (error)
			return error;
	}

	xfs_iext_last(ifp, &icur);
	if (!xfs_iext_get_extent(ifp, &icur, rec))
		*is_empty = 1;
	else
		*is_empty = 0;
	return 0;
}
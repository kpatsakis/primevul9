xfs_bmap_local_to_extents_empty(
	struct xfs_inode	*ip,
	int			whichfork)
{
	struct xfs_ifork	*ifp = XFS_IFORK_PTR(ip, whichfork);

	ASSERT(whichfork != XFS_COW_FORK);
	ASSERT(XFS_IFORK_FORMAT(ip, whichfork) == XFS_DINODE_FMT_LOCAL);
	ASSERT(ifp->if_bytes == 0);
	ASSERT(XFS_IFORK_NEXTENTS(ip, whichfork) == 0);

	xfs_bmap_forkoff_reset(ip, whichfork);
	ifp->if_flags &= ~XFS_IFINLINE;
	ifp->if_flags |= XFS_IFEXTENTS;
	ifp->if_u1.if_root = NULL;
	ifp->if_height = 0;
	XFS_IFORK_FMT_SET(ip, whichfork, XFS_DINODE_FMT_EXTENTS);
}
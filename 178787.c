static inline bool xfs_bmap_needs_btree(struct xfs_inode *ip, int whichfork)
{
	return whichfork != XFS_COW_FORK &&
		XFS_IFORK_FORMAT(ip, whichfork) == XFS_DINODE_FMT_EXTENTS &&
		XFS_IFORK_NEXTENTS(ip, whichfork) >
			XFS_IFORK_MAXEXT(ip, whichfork);
}
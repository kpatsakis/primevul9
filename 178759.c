xfs_bmap_last_offset(
	struct xfs_inode	*ip,
	xfs_fileoff_t		*last_block,
	int			whichfork)
{
	struct xfs_bmbt_irec	rec;
	int			is_empty;
	int			error;

	*last_block = 0;

	if (XFS_IFORK_FORMAT(ip, whichfork) == XFS_DINODE_FMT_LOCAL)
		return 0;

	if (XFS_IFORK_FORMAT(ip, whichfork) != XFS_DINODE_FMT_BTREE &&
	    XFS_IFORK_FORMAT(ip, whichfork) != XFS_DINODE_FMT_EXTENTS)
	       return -EIO;

	error = xfs_bmap_last_extent(NULL, ip, whichfork, &rec, &is_empty);
	if (error || is_empty)
		return error;

	*last_block = rec.br_startoff + rec.br_blockcount;
	return 0;
}
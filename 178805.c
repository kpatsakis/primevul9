xfs_bmap_validate_extent(
	struct xfs_inode	*ip,
	int			whichfork,
	struct xfs_bmbt_irec	*irec)
{
	struct xfs_mount	*mp = ip->i_mount;
	xfs_fsblock_t		endfsb;
	bool			isrt;

	isrt = XFS_IS_REALTIME_INODE(ip);
	endfsb = irec->br_startblock + irec->br_blockcount - 1;
	if (isrt) {
		if (!xfs_verify_rtbno(mp, irec->br_startblock))
			return __this_address;
		if (!xfs_verify_rtbno(mp, endfsb))
			return __this_address;
	} else {
		if (!xfs_verify_fsbno(mp, irec->br_startblock))
			return __this_address;
		if (!xfs_verify_fsbno(mp, endfsb))
			return __this_address;
		if (XFS_FSB_TO_AGNO(mp, irec->br_startblock) !=
		    XFS_FSB_TO_AGNO(mp, endfsb))
			return __this_address;
	}
	if (irec->br_state != XFS_EXT_NORM) {
		if (whichfork != XFS_DATA_FORK)
			return __this_address;
		if (!xfs_sb_version_hasextflgbit(&mp->m_sb))
			return __this_address;
	}
	return NULL;
}
xfs_get_extsz_hint(
	struct xfs_inode	*ip)
{
	/*
	 * No point in aligning allocations if we need to COW to actually
	 * write to them.
	 */
	if (xfs_is_always_cow_inode(ip))
		return 0;
	if ((ip->i_d.di_flags & XFS_DIFLAG_EXTSIZE) && ip->i_d.di_extsize)
		return ip->i_d.di_extsize;
	if (XFS_IS_REALTIME_INODE(ip))
		return ip->i_mount->m_sb.sb_rextsize;
	return 0;
}
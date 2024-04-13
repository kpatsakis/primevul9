xfs_inode_inherit_flags2(
	struct xfs_inode	*ip,
	const struct xfs_inode	*pip)
{
	if (pip->i_d.di_flags2 & XFS_DIFLAG2_COWEXTSIZE) {
		ip->i_d.di_flags2 |= XFS_DIFLAG2_COWEXTSIZE;
		ip->i_d.di_cowextsize = pip->i_d.di_cowextsize;
	}
	if (pip->i_d.di_flags2 & XFS_DIFLAG2_DAX)
		ip->i_d.di_flags2 |= XFS_DIFLAG2_DAX;
}
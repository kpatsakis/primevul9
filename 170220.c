xfs_itruncate_clear_reflink_flags(
	struct xfs_inode	*ip)
{
	struct xfs_ifork	*dfork;
	struct xfs_ifork	*cfork;

	if (!xfs_is_reflink_inode(ip))
		return;
	dfork = XFS_IFORK_PTR(ip, XFS_DATA_FORK);
	cfork = XFS_IFORK_PTR(ip, XFS_COW_FORK);
	if (dfork->if_bytes == 0 && cfork->if_bytes == 0)
		ip->i_d.di_flags2 &= ~XFS_DIFLAG2_REFLINK;
	if (cfork->if_bytes == 0)
		xfs_inode_clear_cowblocks_tag(ip);
}
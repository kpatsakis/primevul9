xfs_trim_extent_eof(
	struct xfs_bmbt_irec	*irec,
	struct xfs_inode	*ip)

{
	xfs_trim_extent(irec, 0, XFS_B_TO_FSB(ip->i_mount,
					      i_size_read(VFS_I(ip))));
}
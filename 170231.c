xfs_ip2xflags(
	struct xfs_inode	*ip)
{
	struct xfs_icdinode	*dic = &ip->i_d;

	return _xfs_dic2xflags(dic->di_flags, dic->di_flags2, XFS_IFORK_Q(ip));
}
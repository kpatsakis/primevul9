static int check_xflags(unsigned int flags)
{
	if (flags & ~(FS_XFLAG_APPEND | FS_XFLAG_IMMUTABLE | FS_XFLAG_NOATIME |
		      FS_XFLAG_NODUMP | FS_XFLAG_SYNC))
		return -EOPNOTSUPP;
	return 0;
}
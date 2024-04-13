static int check_fsflags(unsigned int flags)
{
	if (flags & ~(FS_IMMUTABLE_FL | FS_APPEND_FL | \
		      FS_NOATIME_FL | FS_NODUMP_FL | \
		      FS_SYNC_FL | FS_DIRSYNC_FL | \
		      FS_NOCOMP_FL | FS_COMPR_FL |
		      FS_NOCOW_FL))
		return -EOPNOTSUPP;

	if ((flags & FS_NOCOMP_FL) && (flags & FS_COMPR_FL))
		return -EINVAL;

	return 0;
}
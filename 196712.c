xfs_test_remount_options(
	struct super_block	*sb,
	char			*options)
{
	int			error = 0;
	struct xfs_mount	*tmp_mp;

	tmp_mp = kmem_zalloc(sizeof(*tmp_mp), KM_MAYFAIL);
	if (!tmp_mp)
		return -ENOMEM;

	tmp_mp->m_super = sb;
	error = xfs_parseargs(tmp_mp, options);
	xfs_free_fsname(tmp_mp);
	kmem_free(tmp_mp);

	return error;
}
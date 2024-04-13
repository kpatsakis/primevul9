xfs_free_fsname(
	struct xfs_mount	*mp)
{
	kfree(mp->m_fsname);
	kfree(mp->m_rtname);
	kfree(mp->m_logname);
}
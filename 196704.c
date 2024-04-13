xfs_restore_resvblks(struct xfs_mount *mp)
{
	uint64_t resblks;

	if (mp->m_resblks_save) {
		resblks = mp->m_resblks_save;
		mp->m_resblks_save = 0;
	} else
		resblks = xfs_default_resblks(mp);

	xfs_reserve_blocks(mp, &resblks, NULL);
}
xfs_save_resvblks(struct xfs_mount *mp)
{
	uint64_t resblks = 0;

	mp->m_resblks_save = mp->m_resblks;
	xfs_reserve_blocks(mp, &resblks, NULL);
}
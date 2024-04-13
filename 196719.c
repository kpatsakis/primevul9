xfs_fs_free_cached_objects(
	struct super_block	*sb,
	struct shrink_control	*sc)
{
	return xfs_reclaim_inodes_nr(XFS_M(sb), sc->nr_to_scan);
}
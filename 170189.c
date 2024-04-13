xfs_iunlink_destroy(
	struct xfs_perag	*pag)
{
	bool			freed_anything = false;

	rhashtable_free_and_destroy(&pag->pagi_unlinked_hash,
			xfs_iunlink_free_item, &freed_anything);

	ASSERT(freed_anything == false || XFS_FORCED_SHUTDOWN(pag->pag_mount));
}
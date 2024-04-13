xfs_iunlink_init(
	struct xfs_perag	*pag)
{
	return rhashtable_init(&pag->pagi_unlinked_hash,
			&xfs_iunlink_hash_params);
}
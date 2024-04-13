xfs_iunlink_lookup_backref(
	struct xfs_perag	*pag,
	xfs_agino_t		agino)
{
	struct xfs_iunlink	*iu;

	iu = rhashtable_lookup_fast(&pag->pagi_unlinked_hash, &agino,
			xfs_iunlink_hash_params);
	return iu ? iu->iu_agino : NULLAGINO;
}
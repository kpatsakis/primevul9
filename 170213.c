xfs_iunlink_obj_cmpfn(
	struct rhashtable_compare_arg	*arg,
	const void			*obj)
{
	const xfs_agino_t		*key = arg->key;
	const struct xfs_iunlink	*iu = obj;

	if (iu->iu_next_unlinked != *key)
		return 1;
	return 0;
}
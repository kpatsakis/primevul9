static u64 *rmap_get_next(struct rmap_iterator *iter)
{
	u64 *sptep;

	if (iter->desc) {
		if (iter->pos < PTE_LIST_EXT - 1) {
			++iter->pos;
			sptep = iter->desc->sptes[iter->pos];
			if (sptep)
				goto out;
		}

		iter->desc = iter->desc->more;

		if (iter->desc) {
			iter->pos = 0;
			/* desc->sptes[0] cannot be NULL */
			sptep = iter->desc->sptes[iter->pos];
			goto out;
		}
	}

	return NULL;
out:
	BUG_ON(!is_shadow_present_pte(*sptep));
	return sptep;
}
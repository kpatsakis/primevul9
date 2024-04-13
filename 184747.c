nfp_abm_find_band_for_prio(struct nfp_abm_link *alink, unsigned int prio)
{
	struct nfp_abm_u32_match *iter;

	list_for_each_entry(iter, &alink->dscp_map, list)
		if ((prio & iter->mask) == iter->val)
			return iter->band;

	return alink->def_band;
}
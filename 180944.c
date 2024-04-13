meminit_pfn_in_nid(unsigned long pfn, int node,
		   struct mminit_pfnnid_cache *state)
{
	int nid;

	nid = __early_pfn_to_nid(pfn, state);
	if (nid >= 0 && nid != node)
		return false;
	return true;
}
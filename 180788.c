static inline bool __meminit early_pfn_in_nid(unsigned long pfn, int node)
{
	return meminit_pfn_in_nid(pfn, node, &early_pfnnid_cache);
}
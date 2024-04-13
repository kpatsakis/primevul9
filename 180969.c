static inline bool __meminit early_page_uninitialised(unsigned long pfn)
{
	int nid = early_pfn_to_nid(pfn);

	if (node_online(nid) && pfn >= NODE_DATA(nid)->first_deferred_pfn)
		return true;

	return false;
}
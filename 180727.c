static unsigned long __init find_min_pfn_for_node(int nid)
{
	unsigned long min_pfn = ULONG_MAX;
	unsigned long start_pfn;
	int i;

	for_each_mem_pfn_range(i, nid, &start_pfn, NULL, NULL)
		min_pfn = min(min_pfn, start_pfn);

	if (min_pfn == ULONG_MAX) {
		pr_warn("Could not find start_pfn for node %d\n", nid);
		return 0;
	}

	return min_pfn;
}
unsigned long __init find_min_pfn_with_active_regions(void)
{
	return find_min_pfn_for_node(MAX_NUMNODES);
}
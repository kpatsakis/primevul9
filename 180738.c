static void __meminit __init_single_pfn(unsigned long pfn, unsigned long zone,
					int nid)
{
	return __init_single_page(pfn_to_page(pfn), pfn, zone, nid);
}
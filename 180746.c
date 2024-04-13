unsigned long get_pfnblock_flags_mask(struct page *page, unsigned long pfn,
					unsigned long end_bitidx,
					unsigned long mask)
{
	return __get_pfnblock_flags_mask(page, pfn, end_bitidx, mask);
}
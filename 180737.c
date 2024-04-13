static __always_inline int get_pfnblock_migratetype(struct page *page, unsigned long pfn)
{
	return __get_pfnblock_flags_mask(page, pfn, PB_migrate_end, MIGRATETYPE_MASK);
}
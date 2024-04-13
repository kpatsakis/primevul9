static bool free_unref_page_prepare(struct page *page, unsigned long pfn)
{
	int migratetype;

	if (!free_pcp_prepare(page))
		return false;

	migratetype = get_pfnblock_migratetype(page, pfn);
	set_pcppage_migratetype(page, migratetype);
	return true;
}
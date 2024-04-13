static inline bool page_expected_state(struct page *page,
					unsigned long check_flags)
{
	if (unlikely(atomic_read(&page->_mapcount) != -1))
		return false;

	if (unlikely((unsigned long)page->mapping |
			page_ref_count(page) |
#ifdef CONFIG_MEMCG
			(unsigned long)page->mem_cgroup |
#endif
			(page->flags & check_flags)))
		return false;

	return true;
}
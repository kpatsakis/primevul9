static inline struct page *f2fs_grab_cache_page(struct address_space *mapping,
						pgoff_t index, bool for_write)
{
	struct page *page;

	if (IS_ENABLED(CONFIG_F2FS_FAULT_INJECTION)) {
		if (!for_write)
			page = find_get_page_flags(mapping, index,
							FGP_LOCK | FGP_ACCESSED);
		else
			page = find_lock_page(mapping, index);
		if (page)
			return page;

		if (time_to_inject(F2FS_M_SB(mapping), FAULT_PAGE_ALLOC)) {
			f2fs_show_injection_info(FAULT_PAGE_ALLOC);
			return NULL;
		}
	}

	if (!for_write)
		return grab_cache_page(mapping, index);
	return grab_cache_page_write_begin(mapping, index, AOP_FLAG_NOFS);
}
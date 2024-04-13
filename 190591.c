static inline struct page *f2fs_pagecache_get_page(
				struct address_space *mapping, pgoff_t index,
				int fgp_flags, gfp_t gfp_mask)
{
	if (time_to_inject(F2FS_M_SB(mapping), FAULT_PAGE_GET)) {
		f2fs_show_injection_info(FAULT_PAGE_GET);
		return NULL;
	}

	return pagecache_get_page(mapping, index, fgp_flags, gfp_mask);
}
static void __maybe_unused undo_dev_pagemap(int *nr, int nr_start,
					    unsigned int flags,
					    struct page **pages)
{
	while ((*nr) - nr_start) {
		struct page *page = pages[--(*nr)];

		ClearPageReferenced(page);
		if (flags & FOLL_PIN)
			unpin_user_page(page);
		else
			put_page(page);
	}
}
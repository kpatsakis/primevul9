static int free_tail_pages_check(struct page *head_page, struct page *page)
{
	int ret = 1;

	/*
	 * We rely page->lru.next never has bit 0 set, unless the page
	 * is PageTail(). Let's make sure that's true even for poisoned ->lru.
	 */
	BUILD_BUG_ON((unsigned long)LIST_POISON1 & 1);

	if (!IS_ENABLED(CONFIG_DEBUG_VM)) {
		ret = 0;
		goto out;
	}
	switch (page - head_page) {
	case 1:
		/* the first tail page: ->mapping is compound_mapcount() */
		if (unlikely(compound_mapcount(page))) {
			bad_page(page, "nonzero compound_mapcount", 0);
			goto out;
		}
		break;
	case 2:
		/*
		 * the second tail page: ->mapping is
		 * page_deferred_list().next -- ignore value.
		 */
		break;
	default:
		if (page->mapping != TAIL_MAPPING) {
			bad_page(page, "corrupted mapping in tail page", 0);
			goto out;
		}
		break;
	}
	if (unlikely(!PageTail(page))) {
		bad_page(page, "PageTail not set", 0);
		goto out;
	}
	if (unlikely(compound_head(page) != head_page)) {
		bad_page(page, "compound_head not consistent", 0);
		goto out;
	}
	ret = 0;
out:
	page->mapping = NULL;
	clear_compound_head(page);
	return ret;
}
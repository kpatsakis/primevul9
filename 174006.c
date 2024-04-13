static void __split_huge_page(struct page *page, struct list_head *list,
		unsigned long flags)
{
	struct page *head = compound_head(page);
	struct zone *zone = page_zone(head);
	struct lruvec *lruvec;
	pgoff_t end = -1;
	int i;

	lruvec = mem_cgroup_page_lruvec(head, zone->zone_pgdat);

	/* complete memcg works before add pages to LRU */
	mem_cgroup_split_huge_fixup(head);

	if (!PageAnon(page))
		end = DIV_ROUND_UP(i_size_read(head->mapping->host), PAGE_SIZE);

	for (i = HPAGE_PMD_NR - 1; i >= 1; i--) {
		__split_huge_page_tail(head, i, lruvec, list);
		/* Some pages can be beyond i_size: drop them from page cache */
		if (head[i].index >= end) {
			__ClearPageDirty(head + i);
			__delete_from_page_cache(head + i, NULL);
			if (IS_ENABLED(CONFIG_SHMEM) && PageSwapBacked(head))
				shmem_uncharge(head->mapping->host, 1);
			put_page(head + i);
		}
	}

	ClearPageCompound(head);
	/* See comment in __split_huge_page_tail() */
	if (PageAnon(head)) {
		/* Additional pin to radix tree of swap cache */
		if (PageSwapCache(head))
			page_ref_add(head, 2);
		else
			page_ref_inc(head);
	} else {
		/* Additional pin to radix tree */
		page_ref_add(head, 2);
		spin_unlock(&head->mapping->tree_lock);
	}

	spin_unlock_irqrestore(zone_lru_lock(page_zone(head)), flags);

	unfreeze_page(head);

	for (i = 0; i < HPAGE_PMD_NR; i++) {
		struct page *subpage = head + i;
		if (subpage == page)
			continue;
		unlock_page(subpage);

		/*
		 * Subpages may be freed if there wasn't any mapping
		 * like if add_to_swap() is running on a lru page that
		 * had its mapping zapped. And freeing these pages
		 * requires taking the lru_lock so we do the put_page
		 * of the tail pages after the split is complete.
		 */
		put_page(subpage);
	}
}
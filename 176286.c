static long check_and_migrate_cma_pages(struct task_struct *tsk,
					struct mm_struct *mm,
					unsigned long start,
					unsigned long nr_pages,
					struct page **pages,
					struct vm_area_struct **vmas,
					unsigned int gup_flags)
{
	unsigned long i;
	unsigned long step;
	bool drain_allow = true;
	bool migrate_allow = true;
	LIST_HEAD(cma_page_list);
	long ret = nr_pages;

check_again:
	for (i = 0; i < nr_pages;) {

		struct page *head = compound_head(pages[i]);

		/*
		 * gup may start from a tail page. Advance step by the left
		 * part.
		 */
		step = compound_nr(head) - (pages[i] - head);
		/*
		 * If we get a page from the CMA zone, since we are going to
		 * be pinning these entries, we might as well move them out
		 * of the CMA zone if possible.
		 */
		if (is_migrate_cma_page(head)) {
			if (PageHuge(head))
				isolate_huge_page(head, &cma_page_list);
			else {
				if (!PageLRU(head) && drain_allow) {
					lru_add_drain_all();
					drain_allow = false;
				}

				if (!isolate_lru_page(head)) {
					list_add_tail(&head->lru, &cma_page_list);
					mod_node_page_state(page_pgdat(head),
							    NR_ISOLATED_ANON +
							    page_is_file_lru(head),
							    hpage_nr_pages(head));
				}
			}
		}

		i += step;
	}

	if (!list_empty(&cma_page_list)) {
		/*
		 * drop the above get_user_pages reference.
		 */
		for (i = 0; i < nr_pages; i++)
			put_page(pages[i]);

		if (migrate_pages(&cma_page_list, new_non_cma_page,
				  NULL, 0, MIGRATE_SYNC, MR_CONTIG_RANGE)) {
			/*
			 * some of the pages failed migration. Do get_user_pages
			 * without migration.
			 */
			migrate_allow = false;

			if (!list_empty(&cma_page_list))
				putback_movable_pages(&cma_page_list);
		}
		/*
		 * We did migrate all the pages, Try to get the page references
		 * again migrating any new CMA pages which we failed to isolate
		 * earlier.
		 */
		ret = __get_user_pages_locked(tsk, mm, start, nr_pages,
						   pages, vmas, NULL,
						   gup_flags);

		if ((ret > 0) && migrate_allow) {
			nr_pages = ret;
			drain_allow = true;
			goto check_again;
		}
	}

	return ret;
}
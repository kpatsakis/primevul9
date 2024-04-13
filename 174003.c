int split_huge_page_to_list(struct page *page, struct list_head *list)
{
	struct page *head = compound_head(page);
	struct pglist_data *pgdata = NODE_DATA(page_to_nid(head));
	struct anon_vma *anon_vma = NULL;
	struct address_space *mapping = NULL;
	int count, mapcount, extra_pins, ret;
	bool mlocked;
	unsigned long flags;

	VM_BUG_ON_PAGE(is_huge_zero_page(page), page);
	VM_BUG_ON_PAGE(!PageLocked(page), page);
	VM_BUG_ON_PAGE(!PageCompound(page), page);

	if (PageWriteback(page))
		return -EBUSY;

	if (PageAnon(head)) {
		/*
		 * The caller does not necessarily hold an mmap_sem that would
		 * prevent the anon_vma disappearing so we first we take a
		 * reference to it and then lock the anon_vma for write. This
		 * is similar to page_lock_anon_vma_read except the write lock
		 * is taken to serialise against parallel split or collapse
		 * operations.
		 */
		anon_vma = page_get_anon_vma(head);
		if (!anon_vma) {
			ret = -EBUSY;
			goto out;
		}
		mapping = NULL;
		anon_vma_lock_write(anon_vma);
	} else {
		mapping = head->mapping;

		/* Truncated ? */
		if (!mapping) {
			ret = -EBUSY;
			goto out;
		}

		anon_vma = NULL;
		i_mmap_lock_read(mapping);
	}

	/*
	 * Racy check if we can split the page, before freeze_page() will
	 * split PMDs
	 */
	if (!can_split_huge_page(head, &extra_pins)) {
		ret = -EBUSY;
		goto out_unlock;
	}

	mlocked = PageMlocked(page);
	freeze_page(head);
	VM_BUG_ON_PAGE(compound_mapcount(head), head);

	/* Make sure the page is not on per-CPU pagevec as it takes pin */
	if (mlocked)
		lru_add_drain();

	/* prevent PageLRU to go away from under us, and freeze lru stats */
	spin_lock_irqsave(zone_lru_lock(page_zone(head)), flags);

	if (mapping) {
		void **pslot;

		spin_lock(&mapping->tree_lock);
		pslot = radix_tree_lookup_slot(&mapping->page_tree,
				page_index(head));
		/*
		 * Check if the head page is present in radix tree.
		 * We assume all tail are present too, if head is there.
		 */
		if (radix_tree_deref_slot_protected(pslot,
					&mapping->tree_lock) != head)
			goto fail;
	}

	/* Prevent deferred_split_scan() touching ->_refcount */
	spin_lock(&pgdata->split_queue_lock);
	count = page_count(head);
	mapcount = total_mapcount(head);
	if (!mapcount && page_ref_freeze(head, 1 + extra_pins)) {
		if (!list_empty(page_deferred_list(head))) {
			pgdata->split_queue_len--;
			list_del(page_deferred_list(head));
		}
		if (mapping)
			__dec_node_page_state(page, NR_SHMEM_THPS);
		spin_unlock(&pgdata->split_queue_lock);
		__split_huge_page(page, list, flags);
		if (PageSwapCache(head)) {
			swp_entry_t entry = { .val = page_private(head) };

			ret = split_swap_cluster(entry);
		} else
			ret = 0;
	} else {
		if (IS_ENABLED(CONFIG_DEBUG_VM) && mapcount) {
			pr_alert("total_mapcount: %u, page_count(): %u\n",
					mapcount, count);
			if (PageTail(page))
				dump_page(head, NULL);
			dump_page(page, "total_mapcount(head) > 0");
			BUG();
		}
		spin_unlock(&pgdata->split_queue_lock);
fail:		if (mapping)
			spin_unlock(&mapping->tree_lock);
		spin_unlock_irqrestore(zone_lru_lock(page_zone(head)), flags);
		unfreeze_page(head);
		ret = -EBUSY;
	}

out_unlock:
	if (anon_vma) {
		anon_vma_unlock_write(anon_vma);
		put_anon_vma(anon_vma);
	}
	if (mapping)
		i_mmap_unlock_read(mapping);
out:
	count_vm_event(!ret ? THP_SPLIT_PAGE : THP_SPLIT_PAGE_FAILED);
	return ret;
}
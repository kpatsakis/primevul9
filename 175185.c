static void *__netdev_alloc_frag(unsigned int fragsz, gfp_t gfp_mask)
{
	struct netdev_alloc_cache *nc;
	void *data = NULL;
	int order;
	unsigned long flags;

	local_irq_save(flags);
	nc = &__get_cpu_var(netdev_alloc_cache);
	if (unlikely(!nc->frag.page)) {
refill:
		for (order = NETDEV_FRAG_PAGE_MAX_ORDER; ;) {
			gfp_t gfp = gfp_mask;

			if (order)
				gfp |= __GFP_COMP | __GFP_NOWARN;
			nc->frag.page = alloc_pages(gfp, order);
			if (likely(nc->frag.page))
				break;
			if (--order < 0)
				goto end;
		}
		nc->frag.size = PAGE_SIZE << order;
recycle:
		atomic_set(&nc->frag.page->_count, NETDEV_PAGECNT_MAX_BIAS);
		nc->pagecnt_bias = NETDEV_PAGECNT_MAX_BIAS;
		nc->frag.offset = 0;
	}

	if (nc->frag.offset + fragsz > nc->frag.size) {
		/* avoid unnecessary locked operations if possible */
		if ((atomic_read(&nc->frag.page->_count) == nc->pagecnt_bias) ||
		    atomic_sub_and_test(nc->pagecnt_bias, &nc->frag.page->_count))
			goto recycle;
		goto refill;
	}

	data = page_address(nc->frag.page) + nc->frag.offset;
	nc->frag.offset += fragsz;
	nc->pagecnt_bias--;
end:
	local_irq_restore(flags);
	return data;
}
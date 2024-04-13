void ttm_unmap_and_unpopulate_pages(struct device *dev, struct ttm_dma_tt *tt)
{
	unsigned i, j;

	for (i = 0; i < tt->ttm.num_pages;) {
		struct page *p = tt->ttm.pages[i];
		size_t num_pages = 1;

		if (!tt->dma_address[i] || !tt->ttm.pages[i]) {
			++i;
			continue;
		}

		for (j = i + 1; j < tt->ttm.num_pages; ++j) {
			if (++p != tt->ttm.pages[j])
				break;

			++num_pages;
		}

		dma_unmap_page(dev, tt->dma_address[i], num_pages * PAGE_SIZE,
			       DMA_BIDIRECTIONAL);

		i += num_pages;
	}
	ttm_pool_unpopulate(&tt->ttm);
}
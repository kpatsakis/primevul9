int ttm_populate_and_map_pages(struct device *dev, struct ttm_dma_tt *tt,
					struct ttm_operation_ctx *ctx)
{
	unsigned i, j;
	int r;

	r = ttm_pool_populate(&tt->ttm, ctx);
	if (r)
		return r;

	for (i = 0; i < tt->ttm.num_pages; ++i) {
		struct page *p = tt->ttm.pages[i];
		size_t num_pages = 1;

		for (j = i + 1; j < tt->ttm.num_pages; ++j) {
			if (++p != tt->ttm.pages[j])
				break;

			++num_pages;
		}

		tt->dma_address[i] = dma_map_page(dev, tt->ttm.pages[i],
						  0, num_pages * PAGE_SIZE,
						  DMA_BIDIRECTIONAL);
		if (dma_mapping_error(dev, tt->dma_address[i])) {
			while (i--) {
				dma_unmap_page(dev, tt->dma_address[i],
					       PAGE_SIZE, DMA_BIDIRECTIONAL);
				tt->dma_address[i] = 0;
			}
			ttm_pool_unpopulate(&tt->ttm);
			return -EFAULT;
		}

		for (j = 1; j < num_pages; ++j) {
			tt->dma_address[i + 1] = tt->dma_address[i] + PAGE_SIZE;
			++i;
		}
	}
	return 0;
}
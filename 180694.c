static void *intel_alloc_coherent(struct device *dev, size_t size,
				  dma_addr_t *dma_handle, gfp_t flags,
				  unsigned long attrs)
{
	struct page *page = NULL;
	int order;

	size = PAGE_ALIGN(size);
	order = get_order(size);

	if (!iommu_no_mapping(dev))
		flags &= ~(GFP_DMA | GFP_DMA32);
	else if (dev->coherent_dma_mask < dma_get_required_mask(dev)) {
		if (dev->coherent_dma_mask < DMA_BIT_MASK(32))
			flags |= GFP_DMA;
		else
			flags |= GFP_DMA32;
	}

	if (gfpflags_allow_blocking(flags)) {
		unsigned int count = size >> PAGE_SHIFT;

		page = dma_alloc_from_contiguous(dev, count, order,
						 flags & __GFP_NOWARN);
		if (page && iommu_no_mapping(dev) &&
		    page_to_phys(page) + size > dev->coherent_dma_mask) {
			dma_release_from_contiguous(dev, page, count);
			page = NULL;
		}
	}

	if (!page)
		page = alloc_pages(flags, order);
	if (!page)
		return NULL;
	memset(page_address(page), 0, size);

	*dma_handle = __intel_map_single(dev, page_to_phys(page), size,
					 DMA_BIDIRECTIONAL,
					 dev->coherent_dma_mask);
	if (*dma_handle)
		return page_address(page);
	if (!dma_release_from_contiguous(dev, page, size >> PAGE_SHIFT))
		__free_pages(page, order);

	return NULL;
}
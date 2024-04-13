static dma_addr_t intel_map_page(struct device *dev, struct page *page,
				 unsigned long offset, size_t size,
				 enum dma_data_direction dir,
				 unsigned long attrs)
{
	return __intel_map_single(dev, page_to_phys(page) + offset, size,
				  dir, *dev->dma_mask);
}
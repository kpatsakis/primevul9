static void intel_free_coherent(struct device *dev, size_t size, void *vaddr,
				dma_addr_t dma_handle, unsigned long attrs)
{
	int order;
	struct page *page = virt_to_page(vaddr);

	size = PAGE_ALIGN(size);
	order = get_order(size);

	intel_unmap(dev, dma_handle, size);
	if (!dma_release_from_contiguous(dev, page, size >> PAGE_SHIFT))
		__free_pages(page, order);
}
static void intel_unmap_page(struct device *dev, dma_addr_t dev_addr,
			     size_t size, enum dma_data_direction dir,
			     unsigned long attrs)
{
	intel_unmap(dev, dev_addr, size);
}
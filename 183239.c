static dma_addr_t intel_map_resource(struct device *dev, phys_addr_t phys_addr,
				     size_t size, enum dma_data_direction dir,
				     unsigned long attrs)
{
	return __intel_map_single(dev, phys_addr, size, dir, *dev->dma_mask);
}
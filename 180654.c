static int intel_mapping_error(struct device *dev, dma_addr_t dma_addr)
{
	return !dma_addr;
}
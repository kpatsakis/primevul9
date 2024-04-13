static void intel_unmap_sg(struct device *dev, struct scatterlist *sglist,
			   int nelems, enum dma_data_direction dir,
			   unsigned long attrs)
{
	dma_addr_t startaddr = sg_dma_address(sglist) & PAGE_MASK;
	unsigned long nrpages = 0;
	struct scatterlist *sg;
	int i;

	for_each_sg(sglist, sg, nelems, i) {
		nrpages += aligned_nrpages(sg_dma_address(sg), sg_dma_len(sg));
	}

	intel_unmap(dev, startaddr, nrpages << VTD_PAGE_SHIFT);
}
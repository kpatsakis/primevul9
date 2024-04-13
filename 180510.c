static int intel_nontranslate_map_sg(struct device *hddev,
	struct scatterlist *sglist, int nelems, int dir)
{
	int i;
	struct scatterlist *sg;

	for_each_sg(sglist, sg, nelems, i) {
		BUG_ON(!sg_page(sg));
		sg->dma_address = sg_phys(sg);
		sg->dma_length = sg->length;
	}
	return nelems;
}
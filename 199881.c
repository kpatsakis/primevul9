static void *snd_malloc_dev_pages(struct device *dev, size_t size, dma_addr_t *dma)
{
	int pg;
	void *res;
	gfp_t gfp_flags;

	snd_assert(size > 0, return NULL);
	snd_assert(dma != NULL, return NULL);
	pg = get_order(size);
	gfp_flags = GFP_KERNEL
		| __GFP_COMP	/* compound page lets parts be mapped */
		| __GFP_NORETRY /* don't trigger OOM-killer */
		| __GFP_NOWARN; /* no stack trace print - this call is non-critical */
	res = dma_alloc_coherent(dev, PAGE_SIZE << pg, dma, gfp_flags);
	if (res != NULL)
		inc_snd_pages(pg);

	return res;
}
static void *snd_malloc_sbus_pages(struct device *dev, size_t size,
				   dma_addr_t *dma_addr)
{
	struct sbus_dev *sdev = (struct sbus_dev *)dev;
	int pg;
	void *res;

	snd_assert(size > 0, return NULL);
	snd_assert(dma_addr != NULL, return NULL);
	pg = get_order(size);
	res = sbus_alloc_consistent(sdev, PAGE_SIZE * (1 << pg), dma_addr);
	if (res != NULL)
		inc_snd_pages(pg);
	return res;
}
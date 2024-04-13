static void snd_free_sbus_pages(struct device *dev, size_t size,
				void *ptr, dma_addr_t dma_addr)
{
	struct sbus_dev *sdev = (struct sbus_dev *)dev;
	int pg;

	if (ptr == NULL)
		return;
	pg = get_order(size);
	dec_snd_pages(pg);
	sbus_free_consistent(sdev, PAGE_SIZE * (1 << pg), ptr, dma_addr);
}
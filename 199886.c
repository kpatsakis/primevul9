int snd_dma_alloc_pages_fallback(int type, struct device *device, size_t size,
				 struct snd_dma_buffer *dmab)
{
	int err;

	snd_assert(size > 0, return -ENXIO);
	snd_assert(dmab != NULL, return -ENXIO);

	while ((err = snd_dma_alloc_pages(type, device, size, dmab)) < 0) {
		if (err != -ENOMEM)
			return err;
		size >>= 1;
		if (size <= PAGE_SIZE)
			return -ENOMEM;
	}
	if (! dmab->area)
		return -ENOMEM;
	return 0;
}
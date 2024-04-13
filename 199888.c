int snd_dma_alloc_pages(int type, struct device *device, size_t size,
			struct snd_dma_buffer *dmab)
{
	snd_assert(size > 0, return -ENXIO);
	snd_assert(dmab != NULL, return -ENXIO);

	dmab->dev.type = type;
	dmab->dev.dev = device;
	dmab->bytes = 0;
	switch (type) {
	case SNDRV_DMA_TYPE_CONTINUOUS:
		dmab->area = snd_malloc_pages(size, (unsigned long)device);
		dmab->addr = 0;
		break;
#ifdef CONFIG_SBUS
	case SNDRV_DMA_TYPE_SBUS:
		dmab->area = snd_malloc_sbus_pages(device, size, &dmab->addr);
		break;
#endif
	case SNDRV_DMA_TYPE_DEV:
		dmab->area = snd_malloc_dev_pages(device, size, &dmab->addr);
		break;
	case SNDRV_DMA_TYPE_DEV_SG:
		snd_malloc_sgbuf_pages(device, size, dmab, NULL);
		break;
	default:
		printk(KERN_ERR "snd-malloc: invalid device type %d\n", type);
		dmab->area = NULL;
		dmab->addr = 0;
		return -ENXIO;
	}
	if (! dmab->area)
		return -ENOMEM;
	dmab->bytes = size;
	return 0;
}
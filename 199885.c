void snd_dma_free_pages(struct snd_dma_buffer *dmab)
{
	switch (dmab->dev.type) {
	case SNDRV_DMA_TYPE_CONTINUOUS:
		snd_free_pages(dmab->area, dmab->bytes);
		break;
#ifdef CONFIG_SBUS
	case SNDRV_DMA_TYPE_SBUS:
		snd_free_sbus_pages(dmab->dev.dev, dmab->bytes, dmab->area, dmab->addr);
		break;
#endif
	case SNDRV_DMA_TYPE_DEV:
		snd_free_dev_pages(dmab->dev.dev, dmab->bytes, dmab->area, dmab->addr);
		break;
	case SNDRV_DMA_TYPE_DEV_SG:
		snd_free_sgbuf_pages(dmab);
		break;
	default:
		printk(KERN_ERR "snd-malloc: invalid device type %d\n", dmab->dev.type);
	}
}
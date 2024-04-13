static struct page *go7007_snd_pcm_page(struct snd_pcm_substream *substream,
					unsigned long offset)
{
	return vmalloc_to_page(substream->runtime->dma_area + offset);
}
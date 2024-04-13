static int go7007_snd_hw_free(struct snd_pcm_substream *substream)
{
	struct go7007 *go = snd_pcm_substream_chip(substream);

	go->audio_deliver = NULL;
	if (substream->runtime->dma_bytes > 0)
		vfree(substream->runtime->dma_area);
	substream->runtime->dma_bytes = 0;
	return 0;
}
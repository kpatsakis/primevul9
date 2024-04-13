static int go7007_snd_hw_params(struct snd_pcm_substream *substream,
				struct snd_pcm_hw_params *hw_params)
{
	struct go7007 *go = snd_pcm_substream_chip(substream);
	unsigned int bytes;

	bytes = params_buffer_bytes(hw_params);
	if (substream->runtime->dma_bytes > 0)
		vfree(substream->runtime->dma_area);
	substream->runtime->dma_bytes = 0;
	substream->runtime->dma_area = vmalloc(bytes);
	if (substream->runtime->dma_area == NULL)
		return -ENOMEM;
	substream->runtime->dma_bytes = bytes;
	go->audio_deliver = parse_audio_stream_data;
	return 0;
}
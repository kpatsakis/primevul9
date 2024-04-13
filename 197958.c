static snd_pcm_uframes_t go7007_snd_pcm_pointer(struct snd_pcm_substream *substream)
{
	struct go7007 *go = snd_pcm_substream_chip(substream);
	struct go7007_snd *gosnd = go->snd_context;

	return gosnd->hw_ptr;
}
static int go7007_snd_capture_close(struct snd_pcm_substream *substream)
{
	struct go7007 *go = snd_pcm_substream_chip(substream);
	struct go7007_snd *gosnd = go->snd_context;

	gosnd->substream = NULL;
	return 0;
}
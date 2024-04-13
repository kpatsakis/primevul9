static int go7007_snd_pcm_trigger(struct snd_pcm_substream *substream, int cmd)
{
	struct go7007 *go = snd_pcm_substream_chip(substream);
	struct go7007_snd *gosnd = go->snd_context;

	switch (cmd) {
	case SNDRV_PCM_TRIGGER_START:
		/* Just set a flag to indicate we should signal ALSA when
		 * sound comes in */
		gosnd->capturing = 1;
		return 0;
	case SNDRV_PCM_TRIGGER_STOP:
		gosnd->hw_ptr = gosnd->w_idx = gosnd->avail = 0;
		gosnd->capturing = 0;
		return 0;
	default:
		return -EINVAL;
	}
}
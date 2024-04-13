static int go7007_snd_capture_open(struct snd_pcm_substream *substream)
{
	struct go7007 *go = snd_pcm_substream_chip(substream);
	struct go7007_snd *gosnd = go->snd_context;
	unsigned long flags;
	int r;

	spin_lock_irqsave(&gosnd->lock, flags);
	if (gosnd->substream == NULL) {
		gosnd->substream = substream;
		substream->runtime->hw = go7007_snd_capture_hw;
		r = 0;
	} else
		r = -EBUSY;
	spin_unlock_irqrestore(&gosnd->lock, flags);
	return r;
}
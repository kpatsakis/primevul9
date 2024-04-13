static void parse_audio_stream_data(struct go7007 *go, u8 *buf, int length)
{
	struct go7007_snd *gosnd = go->snd_context;
	struct snd_pcm_runtime *runtime = gosnd->substream->runtime;
	int frames = bytes_to_frames(runtime, length);
	unsigned long flags;

	spin_lock_irqsave(&gosnd->lock, flags);
	gosnd->hw_ptr += frames;
	if (gosnd->hw_ptr >= runtime->buffer_size)
		gosnd->hw_ptr -= runtime->buffer_size;
	gosnd->avail += frames;
	spin_unlock_irqrestore(&gosnd->lock, flags);
	if (gosnd->w_idx + length > runtime->dma_bytes) {
		int cpy = runtime->dma_bytes - gosnd->w_idx;

		memcpy(runtime->dma_area + gosnd->w_idx, buf, cpy);
		length -= cpy;
		buf += cpy;
		gosnd->w_idx = 0;
	}
	memcpy(runtime->dma_area + gosnd->w_idx, buf, length);
	gosnd->w_idx += length;
	spin_lock_irqsave(&gosnd->lock, flags);
	if (gosnd->avail < runtime->period_size) {
		spin_unlock_irqrestore(&gosnd->lock, flags);
		return;
	}
	gosnd->avail -= runtime->period_size;
	spin_unlock_irqrestore(&gosnd->lock, flags);
	if (gosnd->capturing)
		snd_pcm_period_elapsed(gosnd->substream);
}
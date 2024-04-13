static void snd_pcm_xrun_injection_write(struct snd_info_entry *entry,
					 struct snd_info_buffer *buffer)
{
	struct snd_pcm_substream *substream = entry->private_data;
	struct snd_pcm_runtime *runtime;

	snd_pcm_stream_lock_irq(substream);
	runtime = substream->runtime;
	if (runtime && runtime->status->state == SNDRV_PCM_STATE_RUNNING)
		snd_pcm_stop(substream, SNDRV_PCM_STATE_XRUN);
	snd_pcm_stream_unlock_irq(substream);
}
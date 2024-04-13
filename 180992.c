static int snd_pcm_dev_disconnect(struct snd_device *device)
{
	struct snd_pcm *pcm = device->device_data;
	struct snd_pcm_substream *substream;
	int cidx;

	mutex_lock(&register_mutex);
	mutex_lock(&pcm->open_mutex);
	wake_up(&pcm->open_wait);
	list_del_init(&pcm->list);
	for (cidx = 0; cidx < 2; cidx++) {
		for (substream = pcm->streams[cidx].substream; substream; substream = substream->next) {
			snd_pcm_stream_lock_irq(substream);
			if (substream->runtime) {
				if (snd_pcm_running(substream))
					snd_pcm_stop(substream,
						     SNDRV_PCM_STATE_DISCONNECTED);
				/* to be sure, set the state unconditionally */
				substream->runtime->status->state = SNDRV_PCM_STATE_DISCONNECTED;
				wake_up(&substream->runtime->sleep);
				wake_up(&substream->runtime->tsleep);
			}
			snd_pcm_stream_unlock_irq(substream);
		}
	}

	pcm_call_notify(pcm, n_disconnect);
	for (cidx = 0; cidx < 2; cidx++) {
		snd_unregister_device(&pcm->streams[cidx].dev);
		free_chmap(&pcm->streams[cidx]);
	}
	mutex_unlock(&pcm->open_mutex);
	mutex_unlock(&register_mutex);
	return 0;
}
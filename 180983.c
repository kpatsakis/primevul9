static int _snd_pcm_new(struct snd_card *card, const char *id, int device,
		int playback_count, int capture_count, bool internal,
		struct snd_pcm **rpcm)
{
	struct snd_pcm *pcm;
	int err;
	static struct snd_device_ops ops = {
		.dev_free = snd_pcm_dev_free,
		.dev_register =	snd_pcm_dev_register,
		.dev_disconnect = snd_pcm_dev_disconnect,
	};
	static struct snd_device_ops internal_ops = {
		.dev_free = snd_pcm_dev_free,
	};

	if (snd_BUG_ON(!card))
		return -ENXIO;
	if (rpcm)
		*rpcm = NULL;
	pcm = kzalloc(sizeof(*pcm), GFP_KERNEL);
	if (!pcm)
		return -ENOMEM;
	pcm->card = card;
	pcm->device = device;
	pcm->internal = internal;
	mutex_init(&pcm->open_mutex);
	init_waitqueue_head(&pcm->open_wait);
	INIT_LIST_HEAD(&pcm->list);
	if (id)
		strlcpy(pcm->id, id, sizeof(pcm->id));

	err = snd_pcm_new_stream(pcm, SNDRV_PCM_STREAM_PLAYBACK,
				 playback_count);
	if (err < 0)
		goto free_pcm;

	err = snd_pcm_new_stream(pcm, SNDRV_PCM_STREAM_CAPTURE, capture_count);
	if (err < 0)
		goto free_pcm;

	err = snd_device_new(card, SNDRV_DEV_PCM, pcm,
			     internal ? &internal_ops : &ops);
	if (err < 0)
		goto free_pcm;

	if (rpcm)
		*rpcm = pcm;
	return 0;

free_pcm:
	snd_pcm_free(pcm);
	return err;
}
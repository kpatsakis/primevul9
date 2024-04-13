int snd_pcm_new_stream(struct snd_pcm *pcm, int stream, int substream_count)
{
	int idx, err;
	struct snd_pcm_str *pstr = &pcm->streams[stream];
	struct snd_pcm_substream *substream, *prev;

#if IS_ENABLED(CONFIG_SND_PCM_OSS)
	mutex_init(&pstr->oss.setup_mutex);
#endif
	pstr->stream = stream;
	pstr->pcm = pcm;
	pstr->substream_count = substream_count;
	if (!substream_count)
		return 0;

	snd_device_initialize(&pstr->dev, pcm->card);
	pstr->dev.groups = pcm_dev_attr_groups;
	dev_set_name(&pstr->dev, "pcmC%iD%i%c", pcm->card->number, pcm->device,
		     stream == SNDRV_PCM_STREAM_PLAYBACK ? 'p' : 'c');

	if (!pcm->internal) {
		err = snd_pcm_stream_proc_init(pstr);
		if (err < 0) {
			pcm_err(pcm, "Error in snd_pcm_stream_proc_init\n");
			return err;
		}
	}
	prev = NULL;
	for (idx = 0, prev = NULL; idx < substream_count; idx++) {
		substream = kzalloc(sizeof(*substream), GFP_KERNEL);
		if (!substream)
			return -ENOMEM;
		substream->pcm = pcm;
		substream->pstr = pstr;
		substream->number = idx;
		substream->stream = stream;
		sprintf(substream->name, "subdevice #%i", idx);
		substream->buffer_bytes_max = UINT_MAX;
		if (prev == NULL)
			pstr->substream = substream;
		else
			prev->next = substream;

		if (!pcm->internal) {
			err = snd_pcm_substream_proc_init(substream);
			if (err < 0) {
				pcm_err(pcm,
					"Error in snd_pcm_stream_proc_init\n");
				if (prev == NULL)
					pstr->substream = NULL;
				else
					prev->next = NULL;
				kfree(substream);
				return err;
			}
		}
		substream->group = &substream->self_group;
		spin_lock_init(&substream->self_group.lock);
		mutex_init(&substream->self_group.mutex);
		INIT_LIST_HEAD(&substream->self_group.substreams);
		list_add_tail(&substream->link_list, &substream->self_group.substreams);
		atomic_set(&substream->mmap_count, 0);
		prev = substream;
	}
	return 0;
}				
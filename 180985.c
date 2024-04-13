static int snd_pcm_stream_proc_init(struct snd_pcm_str *pstr)
{
	struct snd_pcm *pcm = pstr->pcm;
	struct snd_info_entry *entry;
	char name[16];

	sprintf(name, "pcm%i%c", pcm->device, 
		pstr->stream == SNDRV_PCM_STREAM_PLAYBACK ? 'p' : 'c');
	entry = snd_info_create_card_entry(pcm->card, name,
					   pcm->card->proc_root);
	if (!entry)
		return -ENOMEM;
	entry->mode = S_IFDIR | S_IRUGO | S_IXUGO;
	if (snd_info_register(entry) < 0) {
		snd_info_free_entry(entry);
		return -ENOMEM;
	}
	pstr->proc_root = entry;
	entry = snd_info_create_card_entry(pcm->card, "info", pstr->proc_root);
	if (entry) {
		snd_info_set_text_ops(entry, pstr, snd_pcm_stream_proc_info_read);
		if (snd_info_register(entry) < 0) {
			snd_info_free_entry(entry);
			entry = NULL;
		}
	}
	pstr->proc_info_entry = entry;

#ifdef CONFIG_SND_PCM_XRUN_DEBUG
	entry = snd_info_create_card_entry(pcm->card, "xrun_debug",
					   pstr->proc_root);
	if (entry) {
		entry->c.text.read = snd_pcm_xrun_debug_read;
		entry->c.text.write = snd_pcm_xrun_debug_write;
		entry->mode |= S_IWUSR;
		entry->private_data = pstr;
		if (snd_info_register(entry) < 0) {
			snd_info_free_entry(entry);
			entry = NULL;
		}
	}
	pstr->proc_xrun_debug_entry = entry;
#endif
	return 0;
}
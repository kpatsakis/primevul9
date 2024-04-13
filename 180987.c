static int snd_pcm_substream_proc_init(struct snd_pcm_substream *substream)
{
	struct snd_info_entry *entry;
	struct snd_card *card;
	char name[16];

	card = substream->pcm->card;

	sprintf(name, "sub%i", substream->number);
	entry = snd_info_create_card_entry(card, name,
					   substream->pstr->proc_root);
	if (!entry)
		return -ENOMEM;
	entry->mode = S_IFDIR | S_IRUGO | S_IXUGO;
	if (snd_info_register(entry) < 0) {
		snd_info_free_entry(entry);
		return -ENOMEM;
	}
	substream->proc_root = entry;
	entry = snd_info_create_card_entry(card, "info", substream->proc_root);
	if (entry) {
		snd_info_set_text_ops(entry, substream,
				      snd_pcm_substream_proc_info_read);
		if (snd_info_register(entry) < 0) {
			snd_info_free_entry(entry);
			entry = NULL;
		}
	}
	substream->proc_info_entry = entry;
	entry = snd_info_create_card_entry(card, "hw_params",
					   substream->proc_root);
	if (entry) {
		snd_info_set_text_ops(entry, substream,
				      snd_pcm_substream_proc_hw_params_read);
		if (snd_info_register(entry) < 0) {
			snd_info_free_entry(entry);
			entry = NULL;
		}
	}
	substream->proc_hw_params_entry = entry;
	entry = snd_info_create_card_entry(card, "sw_params",
					   substream->proc_root);
	if (entry) {
		snd_info_set_text_ops(entry, substream,
				      snd_pcm_substream_proc_sw_params_read);
		if (snd_info_register(entry) < 0) {
			snd_info_free_entry(entry);
			entry = NULL;
		}
	}
	substream->proc_sw_params_entry = entry;
	entry = snd_info_create_card_entry(card, "status",
					   substream->proc_root);
	if (entry) {
		snd_info_set_text_ops(entry, substream,
				      snd_pcm_substream_proc_status_read);
		if (snd_info_register(entry) < 0) {
			snd_info_free_entry(entry);
			entry = NULL;
		}
	}
	substream->proc_status_entry = entry;

#ifdef CONFIG_SND_PCM_XRUN_DEBUG
	entry = snd_info_create_card_entry(card, "xrun_injection",
					   substream->proc_root);
	if (entry) {
		entry->private_data = substream;
		entry->c.text.read = NULL;
		entry->c.text.write = snd_pcm_xrun_injection_write;
		entry->mode = S_IFREG | S_IWUSR;
		if (snd_info_register(entry) < 0) {
			snd_info_free_entry(entry);
			entry = NULL;
		}
	}
	substream->proc_xrun_injection_entry = entry;
#endif /* CONFIG_SND_PCM_XRUN_DEBUG */

	return 0;
}
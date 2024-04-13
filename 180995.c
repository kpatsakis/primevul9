static void snd_pcm_free_stream(struct snd_pcm_str * pstr)
{
	struct snd_pcm_substream *substream, *substream_next;
#if IS_ENABLED(CONFIG_SND_PCM_OSS)
	struct snd_pcm_oss_setup *setup, *setupn;
#endif
	substream = pstr->substream;
	while (substream) {
		substream_next = substream->next;
		snd_pcm_timer_done(substream);
		snd_pcm_substream_proc_done(substream);
		kfree(substream);
		substream = substream_next;
	}
	snd_pcm_stream_proc_done(pstr);
#if IS_ENABLED(CONFIG_SND_PCM_OSS)
	for (setup = pstr->oss.setup_list; setup; setup = setupn) {
		setupn = setup->next;
		kfree(setup->task_name);
		kfree(setup);
	}
#endif
	free_chmap(pstr);
	if (pstr->substream_count)
		put_device(&pstr->dev);
}
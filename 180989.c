static int snd_pcm_substream_proc_done(struct snd_pcm_substream *substream)
{
	snd_info_free_entry(substream->proc_info_entry);
	substream->proc_info_entry = NULL;
	snd_info_free_entry(substream->proc_hw_params_entry);
	substream->proc_hw_params_entry = NULL;
	snd_info_free_entry(substream->proc_sw_params_entry);
	substream->proc_sw_params_entry = NULL;
	snd_info_free_entry(substream->proc_status_entry);
	substream->proc_status_entry = NULL;
#ifdef CONFIG_SND_PCM_XRUN_DEBUG
	snd_info_free_entry(substream->proc_xrun_injection_entry);
	substream->proc_xrun_injection_entry = NULL;
#endif
	snd_info_free_entry(substream->proc_root);
	substream->proc_root = NULL;
	return 0;
}
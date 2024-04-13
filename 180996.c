static int snd_pcm_stream_proc_done(struct snd_pcm_str *pstr)
{
#ifdef CONFIG_SND_PCM_XRUN_DEBUG
	snd_info_free_entry(pstr->proc_xrun_debug_entry);
	pstr->proc_xrun_debug_entry = NULL;
#endif
	snd_info_free_entry(pstr->proc_info_entry);
	pstr->proc_info_entry = NULL;
	snd_info_free_entry(pstr->proc_root);
	pstr->proc_root = NULL;
	return 0;
}
void snd_pcm_detach_substream(struct snd_pcm_substream *substream)
{
	struct snd_pcm_runtime *runtime;

	if (PCM_RUNTIME_CHECK(substream))
		return;
	runtime = substream->runtime;
	if (runtime->private_free != NULL)
		runtime->private_free(runtime);
	snd_free_pages((void*)runtime->status,
		       PAGE_ALIGN(sizeof(struct snd_pcm_mmap_status)));
	snd_free_pages((void*)runtime->control,
		       PAGE_ALIGN(sizeof(struct snd_pcm_mmap_control)));
	kfree(runtime->hw_constraints.rules);
	kfree(runtime);
	substream->runtime = NULL;
	put_pid(substream->pid);
	substream->pid = NULL;
	substream->pstr->substream_opened--;
}
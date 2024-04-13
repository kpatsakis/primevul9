static void snd_pcm_substream_proc_status_read(struct snd_info_entry *entry,
					       struct snd_info_buffer *buffer)
{
	struct snd_pcm_substream *substream = entry->private_data;
	struct snd_pcm_runtime *runtime;
	struct snd_pcm_status status;
	int err;

	mutex_lock(&substream->pcm->open_mutex);
	runtime = substream->runtime;
	if (!runtime) {
		snd_iprintf(buffer, "closed\n");
		goto unlock;
	}
	memset(&status, 0, sizeof(status));
	err = snd_pcm_status(substream, &status);
	if (err < 0) {
		snd_iprintf(buffer, "error %d\n", err);
		goto unlock;
	}
	snd_iprintf(buffer, "state: %s\n", snd_pcm_state_name(status.state));
	snd_iprintf(buffer, "owner_pid   : %d\n", pid_vnr(substream->pid));
	snd_iprintf(buffer, "trigger_time: %ld.%09ld\n",
		status.trigger_tstamp.tv_sec, status.trigger_tstamp.tv_nsec);
	snd_iprintf(buffer, "tstamp      : %ld.%09ld\n",
		status.tstamp.tv_sec, status.tstamp.tv_nsec);
	snd_iprintf(buffer, "delay       : %ld\n", status.delay);
	snd_iprintf(buffer, "avail       : %ld\n", status.avail);
	snd_iprintf(buffer, "avail_max   : %ld\n", status.avail_max);
	snd_iprintf(buffer, "-----\n");
	snd_iprintf(buffer, "hw_ptr      : %ld\n", runtime->status->hw_ptr);
	snd_iprintf(buffer, "appl_ptr    : %ld\n", runtime->control->appl_ptr);
 unlock:
	mutex_unlock(&substream->pcm->open_mutex);
}
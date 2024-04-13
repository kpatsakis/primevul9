static int snd_seq_ioctl_system_info(struct snd_seq_client *client, void *arg)
{
	struct snd_seq_system_info *info = arg;

	memset(info, 0, sizeof(*info));
	/* fill the info fields */
	info->queues = SNDRV_SEQ_MAX_QUEUES;
	info->clients = SNDRV_SEQ_MAX_CLIENTS;
	info->ports = SNDRV_SEQ_MAX_PORTS;
	info->channels = 256;	/* fixed limit */
	info->cur_clients = client_usage.cur;
	info->cur_queues = snd_seq_queue_get_cur_queues();

	return 0;
}
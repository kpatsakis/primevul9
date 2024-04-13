static int snd_seq_ioctl_delete_queue(struct snd_seq_client *client, void *arg)
{
	struct snd_seq_queue_info *info = arg;

	return snd_seq_queue_delete(client->number, info->queue);
}
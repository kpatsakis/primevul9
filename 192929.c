static int snd_seq_ioctl_set_queue_tempo(struct snd_seq_client *client,
					 void *arg)
{
	struct snd_seq_queue_tempo *tempo = arg;
	int result;

	result = snd_seq_set_queue_tempo(client->number, tempo);
	return result < 0 ? result : 0;
}
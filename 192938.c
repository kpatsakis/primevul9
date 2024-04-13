static int snd_seq_ioctl_client_id(struct snd_seq_client *client, void *arg)
{
	int *client_id = arg;

	*client_id = client->number;
	return 0;
}
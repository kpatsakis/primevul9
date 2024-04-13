int snd_seq_kernel_client_write_poll(int clientid, struct file *file, poll_table *wait)
{
	struct snd_seq_client *client;

	client = clientptr(clientid);
	if (client == NULL)
		return -ENXIO;

	if (! snd_seq_write_pool_allocated(client))
		return 1;
	if (snd_seq_pool_poll_wait(client->pool, file, wait))
		return 1;
	return 0;
}
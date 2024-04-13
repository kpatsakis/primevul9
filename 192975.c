static int snd_seq_ioctl_query_next_client(struct snd_seq_client *client,
					   void *arg)
{
	struct snd_seq_client_info *info = arg;
	struct snd_seq_client *cptr = NULL;

	/* search for next client */
	info->client++;
	if (info->client < 0)
		info->client = 0;
	for (; info->client < SNDRV_SEQ_MAX_CLIENTS; info->client++) {
		cptr = snd_seq_client_use_ptr(info->client);
		if (cptr)
			break; /* found */
	}
	if (cptr == NULL)
		return -ENOENT;

	get_client_info(cptr, info);
	snd_seq_client_unlock(cptr);

	return 0;
}
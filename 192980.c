static int snd_seq_ioctl_get_client_info(struct snd_seq_client *client,
					 void *arg)
{
	struct snd_seq_client_info *client_info = arg;
	struct snd_seq_client *cptr;

	/* requested client number */
	cptr = snd_seq_client_use_ptr(client_info->client);
	if (cptr == NULL)
		return -ENOENT;		/* don't change !!! */

	get_client_info(cptr, client_info);
	snd_seq_client_unlock(cptr);

	return 0;
}
static int snd_seq_ioctl_set_client_info(struct snd_seq_client *client,
					 void *arg)
{
	struct snd_seq_client_info *client_info = arg;

	/* it is not allowed to set the info fields for an another client */
	if (client->number != client_info->client)
		return -EPERM;
	/* also client type must be set now */
	if (client->type != client_info->type)
		return -EINVAL;

	/* fill the info fields */
	if (client_info->name[0])
		strlcpy(client->name, client_info->name, sizeof(client->name));

	client->filter = client_info->filter;
	client->event_lost = client_info->event_lost;
	memcpy(client->event_filter, client_info->event_filter, 32);

	return 0;
}
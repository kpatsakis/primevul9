static int snd_seq_ioctl_query_next_port(struct snd_seq_client *client,
					 void *arg)
{
	struct snd_seq_port_info *info = arg;
	struct snd_seq_client *cptr;
	struct snd_seq_client_port *port = NULL;

	cptr = snd_seq_client_use_ptr(info->addr.client);
	if (cptr == NULL)
		return -ENXIO;

	/* search for next port */
	info->addr.port++;
	port = snd_seq_port_query_nearest(cptr, info);
	if (port == NULL) {
		snd_seq_client_unlock(cptr);
		return -ENOENT;
	}

	/* get port info */
	info->addr = port->addr;
	snd_seq_get_port_info(port, info);
	snd_seq_port_unlock(port);
	snd_seq_client_unlock(cptr);

	return 0;
}
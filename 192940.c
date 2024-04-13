static int snd_seq_ioctl_get_port_info(struct snd_seq_client *client, void *arg)
{
	struct snd_seq_port_info *info = arg;
	struct snd_seq_client *cptr;
	struct snd_seq_client_port *port;

	cptr = snd_seq_client_use_ptr(info->addr.client);
	if (cptr == NULL)
		return -ENXIO;

	port = snd_seq_port_use_ptr(cptr, info->addr.port);
	if (port == NULL) {
		snd_seq_client_unlock(cptr);
		return -ENOENT;			/* don't change */
	}

	/* get port info */
	snd_seq_get_port_info(port, info);
	snd_seq_port_unlock(port);
	snd_seq_client_unlock(cptr);

	return 0;
}
static int snd_seq_ioctl_create_port(struct snd_seq_client *client, void *arg)
{
	struct snd_seq_port_info *info = arg;
	struct snd_seq_client_port *port;
	struct snd_seq_port_callback *callback;
	int port_idx;

	/* it is not allowed to create the port for an another client */
	if (info->addr.client != client->number)
		return -EPERM;

	port = snd_seq_create_port(client, (info->flags & SNDRV_SEQ_PORT_FLG_GIVEN_PORT) ? info->addr.port : -1);
	if (port == NULL)
		return -ENOMEM;

	if (client->type == USER_CLIENT && info->kernel) {
		port_idx = port->addr.port;
		snd_seq_port_unlock(port);
		snd_seq_delete_port(client, port_idx);
		return -EINVAL;
	}
	if (client->type == KERNEL_CLIENT) {
		if ((callback = info->kernel) != NULL) {
			if (callback->owner)
				port->owner = callback->owner;
			port->private_data = callback->private_data;
			port->private_free = callback->private_free;
			port->event_input = callback->event_input;
			port->c_src.open = callback->subscribe;
			port->c_src.close = callback->unsubscribe;
			port->c_dest.open = callback->use;
			port->c_dest.close = callback->unuse;
		}
	}

	info->addr = port->addr;

	snd_seq_set_port_info(port, info);
	snd_seq_system_client_ev_port_start(port->addr.client, port->addr.port);
	snd_seq_port_unlock(port);

	return 0;
}
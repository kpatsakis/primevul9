static int snd_seq_deliver_single_event(struct snd_seq_client *client,
					struct snd_seq_event *event,
					int filter, int atomic, int hop)
{
	struct snd_seq_client *dest = NULL;
	struct snd_seq_client_port *dest_port = NULL;
	int result = -ENOENT;
	int direct;

	direct = snd_seq_ev_is_direct(event);

	dest = get_event_dest_client(event, filter);
	if (dest == NULL)
		goto __skip;
	dest_port = snd_seq_port_use_ptr(dest, event->dest.port);
	if (dest_port == NULL)
		goto __skip;

	/* check permission */
	if (! check_port_perm(dest_port, SNDRV_SEQ_PORT_CAP_WRITE)) {
		result = -EPERM;
		goto __skip;
	}
		
	if (dest_port->timestamping)
		update_timestamp_of_queue(event, dest_port->time_queue,
					  dest_port->time_real);

	switch (dest->type) {
	case USER_CLIENT:
		if (dest->data.user.fifo)
			result = snd_seq_fifo_event_in(dest->data.user.fifo, event);
		break;

	case KERNEL_CLIENT:
		if (dest_port->event_input == NULL)
			break;
		result = dest_port->event_input(event, direct,
						dest_port->private_data,
						atomic, hop);
		break;
	default:
		break;
	}

  __skip:
	if (dest_port)
		snd_seq_port_unlock(dest_port);
	if (dest)
		snd_seq_client_unlock(dest);

	if (result < 0 && !direct) {
		result = bounce_error_event(client, event, result, atomic, hop);
	}
	return result;
}
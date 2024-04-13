static int port_broadcast_event(struct snd_seq_client *client,
				struct snd_seq_event *event,
				int atomic, int hop)
{
	int num_ev = 0, err, result = 0;
	struct snd_seq_client *dest_client;
	struct snd_seq_client_port *port;

	dest_client = get_event_dest_client(event, SNDRV_SEQ_FILTER_BROADCAST);
	if (dest_client == NULL)
		return 0; /* no matching destination */

	read_lock(&dest_client->ports_lock);
	list_for_each_entry(port, &dest_client->ports_list_head, list) {
		event->dest.port = port->addr.port;
		/* pass NULL as source client to avoid error bounce */
		err = snd_seq_deliver_single_event(NULL, event,
						   SNDRV_SEQ_FILTER_BROADCAST,
						   atomic, hop);
		if (err < 0) {
			/* save first error that occurs and continue */
			if (!result)
				result = err;
			continue;
		}
		num_ev++;
	}
	read_unlock(&dest_client->ports_lock);
	snd_seq_client_unlock(dest_client);
	event->dest.port = SNDRV_SEQ_ADDRESS_BROADCAST; /* restore */
	return (result < 0) ? result : num_ev;
}
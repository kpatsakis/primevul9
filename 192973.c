static int broadcast_event(struct snd_seq_client *client,
			   struct snd_seq_event *event, int atomic, int hop)
{
	int err, result = 0, num_ev = 0;
	int dest;
	struct snd_seq_addr addr;

	addr = event->dest; /* save */

	for (dest = 0; dest < SNDRV_SEQ_MAX_CLIENTS; dest++) {
		/* don't send to itself */
		if (dest == client->number)
			continue;
		event->dest.client = dest;
		event->dest.port = addr.port;
		if (addr.port == SNDRV_SEQ_ADDRESS_BROADCAST)
			err = port_broadcast_event(client, event, atomic, hop);
		else
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
		num_ev += err;
	}
	event->dest = addr; /* restore */
	return (result < 0) ? result : num_ev;
}
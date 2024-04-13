static int bounce_error_event(struct snd_seq_client *client,
			      struct snd_seq_event *event,
			      int err, int atomic, int hop)
{
	struct snd_seq_event bounce_ev;
	int result;

	if (client == NULL ||
	    ! (client->filter & SNDRV_SEQ_FILTER_BOUNCE) ||
	    ! client->accept_input)
		return 0; /* ignored */

	/* set up quoted error */
	memset(&bounce_ev, 0, sizeof(bounce_ev));
	bounce_ev.type = SNDRV_SEQ_EVENT_KERNEL_ERROR;
	bounce_ev.flags = SNDRV_SEQ_EVENT_LENGTH_FIXED;
	bounce_ev.queue = SNDRV_SEQ_QUEUE_DIRECT;
	bounce_ev.source.client = SNDRV_SEQ_CLIENT_SYSTEM;
	bounce_ev.source.port = SNDRV_SEQ_PORT_SYSTEM_ANNOUNCE;
	bounce_ev.dest.client = client->number;
	bounce_ev.dest.port = event->source.port;
	bounce_ev.data.quote.origin = event->dest;
	bounce_ev.data.quote.event = event;
	bounce_ev.data.quote.value = -err; /* use positive value */
	result = snd_seq_deliver_single_event(NULL, &bounce_ev, 0, atomic, hop + 1);
	if (result < 0) {
		client->event_lost++;
		return result;
	}

	return result;
}
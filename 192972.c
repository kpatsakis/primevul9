static int snd_seq_deliver_event(struct snd_seq_client *client, struct snd_seq_event *event,
				 int atomic, int hop)
{
	int result;

	hop++;
	if (hop >= SNDRV_SEQ_MAX_HOPS) {
		pr_debug("ALSA: seq: too long delivery path (%d:%d->%d:%d)\n",
			   event->source.client, event->source.port,
			   event->dest.client, event->dest.port);
		return -EMLINK;
	}

	if (snd_seq_ev_is_variable(event) &&
	    snd_BUG_ON(atomic && (event->data.ext.len & SNDRV_SEQ_EXT_USRPTR)))
		return -EINVAL;

	if (event->queue == SNDRV_SEQ_ADDRESS_SUBSCRIBERS ||
	    event->dest.client == SNDRV_SEQ_ADDRESS_SUBSCRIBERS)
		result = deliver_to_subscribers(client, event, atomic, hop);
#ifdef SUPPORT_BROADCAST
	else if (event->queue == SNDRV_SEQ_ADDRESS_BROADCAST ||
		 event->dest.client == SNDRV_SEQ_ADDRESS_BROADCAST)
		result = broadcast_event(client, event, atomic, hop);
	else if (event->dest.client >= SNDRV_SEQ_MAX_CLIENTS)
		result = multicast_event(client, event, atomic, hop);
	else if (event->dest.port == SNDRV_SEQ_ADDRESS_BROADCAST)
		result = port_broadcast_event(client, event, atomic, hop);
#endif
	else
		result = snd_seq_deliver_single_event(client, event, 0, atomic, hop);

	return result;
}
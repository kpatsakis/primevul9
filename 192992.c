static int deliver_to_subscribers(struct snd_seq_client *client,
				  struct snd_seq_event *event,
				  int atomic, int hop)
{
	struct snd_seq_subscribers *subs;
	int err, result = 0, num_ev = 0;
	struct snd_seq_event event_saved;
	struct snd_seq_client_port *src_port;
	struct snd_seq_port_subs_info *grp;

	src_port = snd_seq_port_use_ptr(client, event->source.port);
	if (src_port == NULL)
		return -EINVAL; /* invalid source port */
	/* save original event record */
	event_saved = *event;
	grp = &src_port->c_src;
	
	/* lock list */
	if (atomic)
		read_lock(&grp->list_lock);
	else
		down_read_nested(&grp->list_mutex, hop);
	list_for_each_entry(subs, &grp->list_head, src_list) {
		/* both ports ready? */
		if (atomic_read(&subs->ref_count) != 2)
			continue;
		event->dest = subs->info.dest;
		if (subs->info.flags & SNDRV_SEQ_PORT_SUBS_TIMESTAMP)
			/* convert time according to flag with subscription */
			update_timestamp_of_queue(event, subs->info.queue,
						  subs->info.flags & SNDRV_SEQ_PORT_SUBS_TIME_REAL);
		err = snd_seq_deliver_single_event(client, event,
						   0, atomic, hop);
		if (err < 0) {
			/* save first error that occurs and continue */
			if (!result)
				result = err;
			continue;
		}
		num_ev++;
		/* restore original event record */
		*event = event_saved;
	}
	if (atomic)
		read_unlock(&grp->list_lock);
	else
		up_read(&grp->list_mutex);
	*event = event_saved; /* restore */
	snd_seq_port_unlock(src_port);
	return (result < 0) ? result : num_ev;
}
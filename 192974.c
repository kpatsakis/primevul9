static struct snd_seq_client *get_event_dest_client(struct snd_seq_event *event,
						    int filter)
{
	struct snd_seq_client *dest;

	dest = snd_seq_client_use_ptr(event->dest.client);
	if (dest == NULL)
		return NULL;
	if (! dest->accept_input)
		goto __not_avail;
	if ((dest->filter & SNDRV_SEQ_FILTER_USE_EVENT) &&
	    ! test_bit(event->type, dest->event_filter))
		goto __not_avail;
	if (filter && !(dest->filter & filter))
		goto __not_avail;

	return dest; /* ok - accessible */
__not_avail:
	snd_seq_client_unlock(dest);
	return NULL;
}
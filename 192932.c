int snd_seq_dispatch_event(struct snd_seq_event_cell *cell, int atomic, int hop)
{
	struct snd_seq_client *client;
	int result;

	if (snd_BUG_ON(!cell))
		return -EINVAL;

	client = snd_seq_client_use_ptr(cell->event.source.client);
	if (client == NULL) {
		snd_seq_cell_free(cell); /* release this cell */
		return -EINVAL;
	}

	if (cell->event.type == SNDRV_SEQ_EVENT_NOTE) {
		/* NOTE event:
		 * the event cell is re-used as a NOTE-OFF event and
		 * enqueued again.
		 */
		struct snd_seq_event tmpev, *ev;

		/* reserve this event to enqueue note-off later */
		tmpev = cell->event;
		tmpev.type = SNDRV_SEQ_EVENT_NOTEON;
		result = snd_seq_deliver_event(client, &tmpev, atomic, hop);

		/*
		 * This was originally a note event.  We now re-use the
		 * cell for the note-off event.
		 */

		ev = &cell->event;
		ev->type = SNDRV_SEQ_EVENT_NOTEOFF;
		ev->flags |= SNDRV_SEQ_PRIORITY_HIGH;

		/* add the duration time */
		switch (ev->flags & SNDRV_SEQ_TIME_STAMP_MASK) {
		case SNDRV_SEQ_TIME_STAMP_TICK:
			ev->time.tick += ev->data.note.duration;
			break;
		case SNDRV_SEQ_TIME_STAMP_REAL:
			/* unit for duration is ms */
			ev->time.time.tv_nsec += 1000000 * (ev->data.note.duration % 1000);
			ev->time.time.tv_sec += ev->data.note.duration / 1000 +
						ev->time.time.tv_nsec / 1000000000;
			ev->time.time.tv_nsec %= 1000000000;
			break;
		}
		ev->data.note.velocity = ev->data.note.off_velocity;

		/* Now queue this cell as the note off event */
		if (snd_seq_enqueue_event(cell, atomic, hop) < 0)
			snd_seq_cell_free(cell); /* release this cell */

	} else {
		/* Normal events:
		 * event cell is freed after processing the event
		 */

		result = snd_seq_deliver_event(client, &cell->event, atomic, hop);
		snd_seq_cell_free(cell);
	}

	snd_seq_client_unlock(client);
	return result;
}
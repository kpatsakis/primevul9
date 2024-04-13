static void smbd_idle_event_handler(struct event_context *ctx,
				    struct timed_event *te,
				    struct timeval now,
				    void *private_data)
{
	struct idle_event *event =
		talloc_get_type_abort(private_data, struct idle_event);

	TALLOC_FREE(event->te);

	DEBUG(10,("smbd_idle_event_handler: %s %p called\n",
		  event->name, event->te));

	if (!event->handler(&now, event->private_data)) {
		DEBUG(10,("smbd_idle_event_handler: %s %p stopped\n",
			  event->name, event->te));
		/* Don't repeat, delete ourselves */
		TALLOC_FREE(event);
		return;
	}

	DEBUG(10,("smbd_idle_event_handler: %s %p rescheduled\n",
		  event->name, event->te));

	event->te = event_add_timed(ctx, event,
				    timeval_sum(&now, &event->interval),
				    smbd_idle_event_handler, event);

	/* We can't do much but fail here. */
	SMB_ASSERT(event->te != NULL);
}
input_parse(struct window_pane *wp)
{
	struct input_ctx		*ictx = wp->ictx;
	const struct input_transition	*itr;
	struct evbuffer			*evb = wp->event->input;
	u_char				*buf;
	size_t				 len, off;

	if (EVBUFFER_LENGTH(evb) == 0)
		return;

	wp->window->flags |= WINDOW_ACTIVITY;
	wp->window->flags &= ~WINDOW_SILENCE;

	if (gettimeofday(&wp->window->activity_time, NULL) != 0)
		fatal("gettimeofday failed");

	/*
	 * Open the screen. Use NULL wp if there is a mode set as don't want to
	 * update the tty.
	 */
	if (wp->mode == NULL)
		screen_write_start(&ictx->ctx, wp, &wp->base);
	else
		screen_write_start(&ictx->ctx, NULL, &wp->base);
	ictx->wp = wp;

	buf = EVBUFFER_DATA(evb);
	len = EVBUFFER_LENGTH(evb);
	notify_input(wp, evb);
	off = 0;

	/* Parse the input. */
	while (off < len) {
		ictx->ch = buf[off++];
		log_debug("%s: '%c' %s", __func__, ictx->ch, ictx->state->name);

		/* Find the transition. */
		itr = ictx->state->transitions;
		while (itr->first != -1 && itr->last != -1) {
			if (ictx->ch >= itr->first && ictx->ch <= itr->last)
				break;
			itr++;
		}
		if (itr->first == -1 || itr->last == -1) {
			/* No transition? Eh? */
			fatalx("No transition from state!");
		}

		/*
		 * Execute the handler, if any. Don't switch state if it
		 * returns non-zero.
		 */
		if (itr->handler != NULL && itr->handler(ictx) != 0)
			continue;

		/* And switch state, if necessary. */
		if (itr->state != NULL)
			input_set_state(wp, itr);

		/* If not in ground state, save input. */
		if (ictx->state != &input_state_ground)
			evbuffer_add(ictx->since_ground, &ictx->ch, 1);
	}

	/* Close the screen. */
	screen_write_stop(&ictx->ctx);

	evbuffer_drain(evb, len);
}
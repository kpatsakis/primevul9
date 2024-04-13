input_parse(struct input_ctx *ictx, u_char *buf, size_t len)
{
	struct screen_write_ctx		*sctx = &ictx->ctx;
	const struct input_state	*state = NULL;
	const struct input_transition	*itr = NULL;
	size_t				 off = 0;

	/* Parse the input. */
	while (off < len) {
		ictx->ch = buf[off++];

		/* Find the transition. */
		if (ictx->state != state ||
		    itr == NULL ||
		    ictx->ch < itr->first ||
		    ictx->ch > itr->last) {
			itr = ictx->state->transitions;
			while (itr->first != -1 && itr->last != -1) {
				if (ictx->ch >= itr->first &&
				    ictx->ch <= itr->last)
					break;
				itr++;
			}
			if (itr->first == -1 || itr->last == -1) {
				/* No transition? Eh? */
				fatalx("no transition from state");
			}
		}
		state = ictx->state;

		/*
		 * Any state except print stops the current collection. This is
		 * an optimization to avoid checking if the attributes have
		 * changed for every character. It will stop unnecessarily for
		 * sequences that don't make a terminal change, but they should
		 * be the minority.
		 */
		if (itr->handler != input_print)
			screen_write_collect_end(sctx);

		/*
		 * Execute the handler, if any. Don't switch state if it
		 * returns non-zero.
		 */
		if (itr->handler != NULL && itr->handler(ictx) != 0)
			continue;

		/* And switch state, if necessary. */
		if (itr->state != NULL)
			input_set_state(ictx, itr);

		/* If not in ground state, save input. */
		if (ictx->state != &input_state_ground)
			evbuffer_add(ictx->since_ground, &ictx->ch, 1);
	}
}
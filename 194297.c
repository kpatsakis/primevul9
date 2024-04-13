static unsigned long calculate_next_time(struct ml_effect_state *state)
{
	const struct ff_envelope *envelope = get_envelope(state->effect);
	unsigned long attack_stop, fade_start, next_fade;

	if (envelope->attack_length) {
		attack_stop = state->play_at +
			msecs_to_jiffies(envelope->attack_length);
		if (time_before(state->adj_at, attack_stop))
			return state->adj_at +
					msecs_to_jiffies(FF_ENVELOPE_INTERVAL);
	}

	if (state->effect->replay.length) {
		if (envelope->fade_length) {
			/* check when fading should start */
			fade_start = state->stop_at -
					msecs_to_jiffies(envelope->fade_length);

			if (time_before(state->adj_at, fade_start))
				return fade_start;

			/* already fading, advance to next checkpoint */
			next_fade = state->adj_at +
					msecs_to_jiffies(FF_ENVELOPE_INTERVAL);
			if (time_before(next_fade, state->stop_at))
				return next_fade;
		}

		return state->stop_at;
	}

	return state->play_at;
}
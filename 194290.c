static void ml_schedule_timer(struct ml_device *ml)
{
	struct ml_effect_state *state;
	unsigned long now = jiffies;
	unsigned long earliest = 0;
	unsigned long next_at;
	int events = 0;
	int i;

	pr_debug("calculating next timer\n");

	for (i = 0; i < FF_MEMLESS_EFFECTS; i++) {

		state = &ml->states[i];

		if (!test_bit(FF_EFFECT_STARTED, &state->flags))
			continue;

		if (test_bit(FF_EFFECT_PLAYING, &state->flags))
			next_at = calculate_next_time(state);
		else
			next_at = state->play_at;

		if (time_before_eq(now, next_at) &&
		    (++events == 1 || time_before(next_at, earliest)))
			earliest = next_at;
	}

	if (!events) {
		pr_debug("no actions\n");
		del_timer(&ml->timer);
	} else {
		pr_debug("timer set\n");
		mod_timer(&ml->timer, earliest);
	}
}
static int ml_ff_playback(struct input_dev *dev, int effect_id, int value)
{
	struct ml_device *ml = dev->ff->private;
	struct ml_effect_state *state = &ml->states[effect_id];

	if (value > 0) {
		pr_debug("initiated play\n");

		__set_bit(FF_EFFECT_STARTED, &state->flags);
		state->count = value;
		state->play_at = jiffies +
				 msecs_to_jiffies(state->effect->replay.delay);
		state->stop_at = state->play_at +
				 msecs_to_jiffies(state->effect->replay.length);
		state->adj_at = state->play_at;

	} else {
		pr_debug("initiated stop\n");

		if (test_bit(FF_EFFECT_PLAYING, &state->flags))
			__set_bit(FF_EFFECT_ABORTING, &state->flags);
		else
			__clear_bit(FF_EFFECT_STARTED, &state->flags);
	}

	ml_play_effects(ml);

	return 0;
}
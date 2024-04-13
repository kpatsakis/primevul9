static int ml_get_combo_effect(struct ml_device *ml,
			       unsigned long *effect_handled,
			       struct ff_effect *combo_effect)
{
	struct ff_effect *effect;
	struct ml_effect_state *state;
	int effect_type;
	int i;

	memset(combo_effect, 0, sizeof(struct ff_effect));

	for (i = 0; i < FF_MEMLESS_EFFECTS; i++) {
		if (__test_and_set_bit(i, effect_handled))
			continue;

		state = &ml->states[i];
		effect = state->effect;

		if (!test_bit(FF_EFFECT_STARTED, &state->flags))
			continue;

		if (time_before(jiffies, state->play_at))
			continue;

		/*
		 * here we have started effects that are either
		 * currently playing (and may need be aborted)
		 * or need to start playing.
		 */
		effect_type = get_compatible_type(ml->dev->ff, effect->type);
		if (combo_effect->type != effect_type) {
			if (combo_effect->type != 0) {
				__clear_bit(i, effect_handled);
				continue;
			}
			combo_effect->type = effect_type;
		}

		if (__test_and_clear_bit(FF_EFFECT_ABORTING, &state->flags)) {
			__clear_bit(FF_EFFECT_PLAYING, &state->flags);
			__clear_bit(FF_EFFECT_STARTED, &state->flags);
		} else if (effect->replay.length &&
			   time_after_eq(jiffies, state->stop_at)) {

			__clear_bit(FF_EFFECT_PLAYING, &state->flags);

			if (--state->count <= 0) {
				__clear_bit(FF_EFFECT_STARTED, &state->flags);
			} else {
				state->play_at = jiffies +
					msecs_to_jiffies(effect->replay.delay);
				state->stop_at = state->play_at +
					msecs_to_jiffies(effect->replay.length);
			}
		} else {
			__set_bit(FF_EFFECT_PLAYING, &state->flags);
			state->adj_at = jiffies;
			ml_combine_effects(combo_effect, state, ml->gain);
		}
	}

	return combo_effect->type != 0;
}
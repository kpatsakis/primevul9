static int ml_ff_upload(struct input_dev *dev,
			struct ff_effect *effect, struct ff_effect *old)
{
	struct ml_device *ml = dev->ff->private;
	struct ml_effect_state *state = &ml->states[effect->id];

	spin_lock_irq(&dev->event_lock);

	if (test_bit(FF_EFFECT_STARTED, &state->flags)) {
		__clear_bit(FF_EFFECT_PLAYING, &state->flags);
		state->play_at = jiffies +
				 msecs_to_jiffies(state->effect->replay.delay);
		state->stop_at = state->play_at +
				 msecs_to_jiffies(state->effect->replay.length);
		state->adj_at = state->play_at;
		ml_schedule_timer(ml);
	}

	spin_unlock_irq(&dev->event_lock);

	return 0;
}
static void ml_ff_set_gain(struct input_dev *dev, u16 gain)
{
	struct ml_device *ml = dev->ff->private;
	int i;

	ml->gain = gain;

	for (i = 0; i < FF_MEMLESS_EFFECTS; i++)
		__clear_bit(FF_EFFECT_PLAYING, &ml->states[i].flags);

	ml_play_effects(ml);
}
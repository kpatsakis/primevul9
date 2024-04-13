int input_ff_create_memless(struct input_dev *dev, void *data,
		int (*play_effect)(struct input_dev *, void *, struct ff_effect *))
{
	struct ml_device *ml;
	struct ff_device *ff;
	int error;
	int i;

	ml = kzalloc(sizeof(struct ml_device), GFP_KERNEL);
	if (!ml)
		return -ENOMEM;

	ml->dev = dev;
	ml->private = data;
	ml->play_effect = play_effect;
	ml->gain = 0xffff;
	timer_setup(&ml->timer, ml_effect_timer, 0);

	set_bit(FF_GAIN, dev->ffbit);

	error = input_ff_create(dev, FF_MEMLESS_EFFECTS);
	if (error) {
		kfree(ml);
		return error;
	}

	ff = dev->ff;
	ff->private = ml;
	ff->upload = ml_ff_upload;
	ff->playback = ml_ff_playback;
	ff->set_gain = ml_ff_set_gain;
	ff->destroy = ml_ff_destroy;

	/* we can emulate periodic effects with RUMBLE */
	if (test_bit(FF_RUMBLE, ff->ffbit)) {
		set_bit(FF_PERIODIC, dev->ffbit);
		set_bit(FF_SINE, dev->ffbit);
		set_bit(FF_TRIANGLE, dev->ffbit);
		set_bit(FF_SQUARE, dev->ffbit);
	}

	for (i = 0; i < FF_MEMLESS_EFFECTS; i++)
		ml->states[i].effect = &ff->effects[i];

	return 0;
}
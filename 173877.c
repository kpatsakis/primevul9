static int hidpp_ff_playback(struct input_dev *dev, int effect_id, int value)
{
	struct hidpp_ff_private_data *data = dev->ff->private;
	u8 params[2];

	params[1] = value ? HIDPP_FF_EFFECT_STATE_PLAY : HIDPP_FF_EFFECT_STATE_STOP;

	dbg_hid("St%sing playback of effect %d.\n", value?"art":"opp", effect_id);

	return hidpp_ff_queue_work(data, effect_id, HIDPP_FF_SET_EFFECT_STATE, params, ARRAY_SIZE(params));
}
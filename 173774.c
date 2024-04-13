static int ms_play_effect(struct input_dev *dev, void *data,
			  struct ff_effect *effect)
{
	struct hid_device *hid = input_get_drvdata(dev);
	struct ms_data *ms = hid_get_drvdata(hid);

	if (effect->type != FF_RUMBLE)
		return 0;

	/*
	 * Magnitude is 0..100 so scale the 16-bit input here
	 */
	ms->strong = ((u32) effect->u.rumble.strong_magnitude * 100) / U16_MAX;
	ms->weak = ((u32) effect->u.rumble.weak_magnitude * 100) / U16_MAX;

	schedule_work(&ms->ff_worker);
	return 0;
}
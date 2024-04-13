static int holtekff_play(struct input_dev *dev, void *data,
			 struct ff_effect *effect)
{
	struct hid_device *hid = input_get_drvdata(dev);
	struct holtekff_device *holtekff = data;
	int left, right;
	/* effect type 1, length 65535 msec */
	u8 buf[HOLTEKFF_MSG_LENGTH] =
		{ 0x01, 0x01, 0xff, 0xff, 0x10, 0xe0, 0x00 };

	left = effect->u.rumble.strong_magnitude;
	right = effect->u.rumble.weak_magnitude;
	dbg_hid("called with 0x%04x 0x%04x\n", left, right);

	if (!left && !right) {
		holtekff_send(holtekff, hid, stop_all6);
		return 0;
	}

	if (left)
		buf[1] |= 0x80;
	if (right)
		buf[1] |= 0x40;

	/* The device takes a single magnitude, so we just sum them up. */
	buf[6] = min(0xf, (left >> 12) + (right >> 12));

	holtekff_send(holtekff, hid, buf);
	holtekff_send(holtekff, hid, start_effect_1);

	return 0;
}
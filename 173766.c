static int zpff_play(struct input_dev *dev, void *data,
			 struct ff_effect *effect)
{
	struct hid_device *hid = input_get_drvdata(dev);
	struct zpff_device *zpff = data;
	int left, right;

	/*
	 * The following is specified the other way around in the Zeroplus
	 * datasheet but the order below is correct for the XFX Executioner;
	 * however it is possible that the XFX Executioner is an exception
	 */

	left = effect->u.rumble.strong_magnitude;
	right = effect->u.rumble.weak_magnitude;
	dbg_hid("called with 0x%04x 0x%04x\n", left, right);

	left = left * 0x7f / 0xffff;
	right = right * 0x7f / 0xffff;

	zpff->report->field[2]->value[0] = left;
	zpff->report->field[3]->value[0] = right;
	dbg_hid("running with 0x%02x 0x%02x\n", left, right);
	hid_hw_request(hid, zpff->report, HID_REQ_SET_REPORT);

	return 0;
}
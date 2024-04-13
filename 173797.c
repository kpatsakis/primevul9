static int drff_play(struct input_dev *dev, void *data,
				 struct ff_effect *effect)
{
	struct hid_device *hid = input_get_drvdata(dev);
	struct drff_device *drff = data;
	int strong, weak;

	strong = effect->u.rumble.strong_magnitude;
	weak = effect->u.rumble.weak_magnitude;

	dbg_hid("called with 0x%04x 0x%04x", strong, weak);

	if (strong || weak) {
		strong = strong * 0xff / 0xffff;
		weak = weak * 0xff / 0xffff;

		/* While reverse engineering this device, I found that when
		   this value is set, it causes the strong rumble to function
		   at a near maximum speed, so we'll bypass it. */
		if (weak == 0x0a)
			weak = 0x0b;

		drff->report->field[0]->value[0] = 0x51;
		drff->report->field[0]->value[1] = 0x00;
		drff->report->field[0]->value[2] = weak;
		drff->report->field[0]->value[4] = strong;
		hid_hw_request(hid, drff->report, HID_REQ_SET_REPORT);

		drff->report->field[0]->value[0] = 0xfa;
		drff->report->field[0]->value[1] = 0xfe;
	} else {
		drff->report->field[0]->value[0] = 0xf3;
		drff->report->field[0]->value[1] = 0x00;
	}

	drff->report->field[0]->value[2] = 0x00;
	drff->report->field[0]->value[4] = 0x00;
	dbg_hid("running with 0x%02x 0x%02x", strong, weak);
	hid_hw_request(hid, drff->report, HID_REQ_SET_REPORT);

	return 0;
}
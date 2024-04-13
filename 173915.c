static int hid_gaff_play(struct input_dev *dev, void *data,
			 struct ff_effect *effect)
{
	struct hid_device *hid = input_get_drvdata(dev);
	struct gaff_device *gaff = data;
	int left, right;

	left = effect->u.rumble.strong_magnitude;
	right = effect->u.rumble.weak_magnitude;

	dbg_hid("called with 0x%04x 0x%04x", left, right);

	left = left * 0xfe / 0xffff;
	right = right * 0xfe / 0xffff;

	gaff->report->field[0]->value[0] = 0x51;
	gaff->report->field[0]->value[1] = 0x0;
	gaff->report->field[0]->value[2] = right;
	gaff->report->field[0]->value[3] = 0;
	gaff->report->field[0]->value[4] = left;
	gaff->report->field[0]->value[5] = 0;
	dbg_hid("running with 0x%02x 0x%02x", left, right);
	hid_hw_request(hid, gaff->report, HID_REQ_SET_REPORT);

	gaff->report->field[0]->value[0] = 0xfa;
	gaff->report->field[0]->value[1] = 0xfe;
	gaff->report->field[0]->value[2] = 0x0;
	gaff->report->field[0]->value[4] = 0x0;

	hid_hw_request(hid, gaff->report, HID_REQ_SET_REPORT);

	return 0;
}
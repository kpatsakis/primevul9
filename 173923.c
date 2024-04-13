static int axff_play(struct input_dev *dev, void *data, struct ff_effect *effect)
{
	struct hid_device *hid = input_get_drvdata(dev);
	struct axff_device *axff = data;
	struct hid_report *report = axff->report;
	int field_count = 0;
	int left, right;
	int i, j;

	left = effect->u.rumble.strong_magnitude;
	right = effect->u.rumble.weak_magnitude;

	dbg_hid("called with 0x%04x 0x%04x", left, right);

	left = left * 0xff / 0xffff;
	right = right * 0xff / 0xffff;

	for (i = 0; i < report->maxfield; i++) {
		for (j = 0; j < report->field[i]->report_count; j++) {
			report->field[i]->value[j] =
				field_count % 2 ? right : left;
			field_count++;
		}
	}

	dbg_hid("running with 0x%02x 0x%02x", left, right);
	hid_hw_request(hid, axff->report, HID_REQ_SET_REPORT);

	return 0;
}
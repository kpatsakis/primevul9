static int hid_lgff_play(struct input_dev *dev, void *data, struct ff_effect *effect)
{
	struct hid_device *hid = input_get_drvdata(dev);
	struct list_head *report_list = &hid->report_enum[HID_OUTPUT_REPORT].report_list;
	struct hid_report *report = list_entry(report_list->next, struct hid_report, list);
	int x, y;
	unsigned int left, right;

#define CLAMP(x) if (x < 0) x = 0; if (x > 0xff) x = 0xff

	switch (effect->type) {
	case FF_CONSTANT:
		x = effect->u.ramp.start_level + 0x7f;	/* 0x7f is center */
		y = effect->u.ramp.end_level + 0x7f;
		CLAMP(x);
		CLAMP(y);
		report->field[0]->value[0] = 0x51;
		report->field[0]->value[1] = 0x08;
		report->field[0]->value[2] = x;
		report->field[0]->value[3] = y;
		dbg_hid("(x, y)=(%04x, %04x)\n", x, y);
		hid_hw_request(hid, report, HID_REQ_SET_REPORT);
		break;

	case FF_RUMBLE:
		right = effect->u.rumble.strong_magnitude;
		left = effect->u.rumble.weak_magnitude;
		right = right * 0xff / 0xffff;
		left = left * 0xff / 0xffff;
		CLAMP(left);
		CLAMP(right);
		report->field[0]->value[0] = 0x42;
		report->field[0]->value[1] = 0x00;
		report->field[0]->value[2] = left;
		report->field[0]->value[3] = right;
		dbg_hid("(left, right)=(%04x, %04x)\n", left, right);
		hid_hw_request(hid, report, HID_REQ_SET_REPORT);
		break;
	}
	return 0;
}
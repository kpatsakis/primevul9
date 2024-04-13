static int hid_lg3ff_play(struct input_dev *dev, void *data,
			 struct ff_effect *effect)
{
	struct hid_device *hid = input_get_drvdata(dev);
	struct list_head *report_list = &hid->report_enum[HID_OUTPUT_REPORT].report_list;
	struct hid_report *report = list_entry(report_list->next, struct hid_report, list);
	int x, y;

/*
 * Available values in the field should always be 63, but we only use up to
 * 35. Instead, clear the entire area, however big it is.
 */
	memset(report->field[0]->value, 0,
	       sizeof(__s32) * report->field[0]->report_count);

	switch (effect->type) {
	case FF_CONSTANT:
/*
 * Already clamped in ff_memless
 * 0 is center (different then other logitech)
 */
		x = effect->u.ramp.start_level;
		y = effect->u.ramp.end_level;

		/* send command byte */
		report->field[0]->value[0] = 0x51;

/*
 * Sign backwards from other Force3d pro
 * which get recast here in two's complement 8 bits
 */
		report->field[0]->value[1] = (unsigned char)(-x);
		report->field[0]->value[31] = (unsigned char)(-y);

		hid_hw_request(hid, report, HID_REQ_SET_REPORT);
		break;
	}
	return 0;
}
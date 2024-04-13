static int tmff_play(struct input_dev *dev, void *data,
		struct ff_effect *effect)
{
	struct hid_device *hid = input_get_drvdata(dev);
	struct tmff_device *tmff = data;
	struct hid_field *ff_field = tmff->ff_field;
	int x, y;
	int left, right;	/* Rumbling */
	int motor_swap;

	switch (effect->type) {
	case FF_CONSTANT:
		x = tmff_scale_s8(effect->u.ramp.start_level,
					ff_field->logical_minimum,
					ff_field->logical_maximum);
		y = tmff_scale_s8(effect->u.ramp.end_level,
					ff_field->logical_minimum,
					ff_field->logical_maximum);

		dbg_hid("(x, y)=(%04x, %04x)\n", x, y);
		ff_field->value[0] = x;
		ff_field->value[1] = y;
		hid_hw_request(hid, tmff->report, HID_REQ_SET_REPORT);
		break;

	case FF_RUMBLE:
		left = tmff_scale_u16(effect->u.rumble.weak_magnitude,
					ff_field->logical_minimum,
					ff_field->logical_maximum);
		right = tmff_scale_u16(effect->u.rumble.strong_magnitude,
					ff_field->logical_minimum,
					ff_field->logical_maximum);

		/* 2-in-1 strong motor is left */
		if (hid->product == THRUSTMASTER_DEVICE_ID_2_IN_1_DT) {
			motor_swap = left;
			left = right;
			right = motor_swap;
		}

		dbg_hid("(left,right)=(%08x, %08x)\n", left, right);
		ff_field->value[0] = left;
		ff_field->value[1] = right;
		hid_hw_request(hid, tmff->report, HID_REQ_SET_REPORT);
		break;
	}
	return 0;
}
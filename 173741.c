static int wtp_raw_event(struct hid_device *hdev, u8 *data, int size)
{
	struct hidpp_device *hidpp = hid_get_drvdata(hdev);
	struct wtp_data *wd = hidpp->private_data;
	struct hidpp_report *report = (struct hidpp_report *)data;
	struct hidpp_touchpad_raw_xy raw;

	if (!wd || !hidpp->input)
		return 1;

	switch (data[0]) {
	case 0x02:
		if (size < 2) {
			hid_err(hdev, "Received HID report of bad size (%d)",
				size);
			return 1;
		}
		if (hidpp->quirks & HIDPP_QUIRK_WTP_PHYSICAL_BUTTONS) {
			input_event(hidpp->input, EV_KEY, BTN_LEFT,
					!!(data[1] & 0x01));
			input_event(hidpp->input, EV_KEY, BTN_RIGHT,
					!!(data[1] & 0x02));
			input_sync(hidpp->input);
			return 0;
		} else {
			if (size < 21)
				return 1;
			return wtp_mouse_raw_xy_event(hidpp, &data[7]);
		}
	case REPORT_ID_HIDPP_LONG:
		/* size is already checked in hidpp_raw_event. */
		if ((report->fap.feature_index != wd->mt_feature_index) ||
		    (report->fap.funcindex_clientid != EVENT_TOUCHPAD_RAW_XY))
			return 1;
		hidpp_touchpad_raw_xy_event(hidpp, data + 4, &raw);

		wtp_send_raw_xy_event(hidpp, &raw);
		return 0;
	}

	return 0;
}
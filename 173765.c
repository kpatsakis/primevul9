static int ms_surface_dial_quirk(struct hid_input *hi, struct hid_field *field,
		struct hid_usage *usage, unsigned long **bit, int *max)
{
	switch (usage->hid & HID_USAGE_PAGE) {
	case 0xff070000:
		/* fall-through */
	case HID_UP_DIGITIZER:
		/* ignore those axis */
		return -1;
	case HID_UP_GENDESK:
		switch (usage->hid) {
		case HID_GD_X:
			/* fall-through */
		case HID_GD_Y:
			/* fall-through */
		case HID_GD_RFKILL_BTN:
			/* ignore those axis */
			return -1;
		}
	}

	return 0;
}
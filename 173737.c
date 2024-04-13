static int sony_mapping(struct hid_device *hdev, struct hid_input *hi,
			struct hid_field *field, struct hid_usage *usage,
			unsigned long **bit, int *max)
{
	struct sony_sc *sc = hid_get_drvdata(hdev);

	if (sc->quirks & BUZZ_CONTROLLER) {
		unsigned int key = usage->hid & HID_USAGE;

		if ((usage->hid & HID_USAGE_PAGE) != HID_UP_BUTTON)
			return -1;

		switch (usage->collection_index) {
		case 1:
			if (key >= ARRAY_SIZE(buzz_keymap))
				return -1;

			key = buzz_keymap[key];
			if (!key)
				return -1;
			break;
		default:
			return -1;
		}

		hid_map_usage_clear(hi, usage, bit, max, EV_KEY, key);
		return 1;
	}

	if (sc->quirks & PS3REMOTE)
		return ps3remote_mapping(hdev, hi, field, usage, bit, max);

	if (sc->quirks & NAVIGATION_CONTROLLER)
		return navigation_mapping(hdev, hi, field, usage, bit, max);

	if (sc->quirks & SIXAXIS_CONTROLLER)
		return sixaxis_mapping(hdev, hi, field, usage, bit, max);

	if (sc->quirks & DUALSHOCK4_CONTROLLER)
		return ds4_mapping(hdev, hi, field, usage, bit, max);


	/* Let hid-core decide for the others */
	return 0;
}
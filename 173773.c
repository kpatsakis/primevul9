static int ds4_mapping(struct hid_device *hdev, struct hid_input *hi,
		       struct hid_field *field, struct hid_usage *usage,
		       unsigned long **bit, int *max)
{
	if ((usage->hid & HID_USAGE_PAGE) == HID_UP_BUTTON) {
		unsigned int key = usage->hid & HID_USAGE;

		if (key >= ARRAY_SIZE(ds4_keymap))
			return -1;

		key = ds4_keymap[key];
		hid_map_usage_clear(hi, usage, bit, max, EV_KEY, key);
		return 1;
	} else if ((usage->hid & HID_USAGE_PAGE) == HID_UP_GENDESK) {
		unsigned int abs = usage->hid & HID_USAGE;

		/* Let the HID parser deal with the HAT. */
		if (usage->hid == HID_GD_HATSWITCH)
			return 0;

		if (abs >= ARRAY_SIZE(ds4_absmap))
			return -1;

		abs = ds4_absmap[abs];
		hid_map_usage_clear(hi, usage, bit, max, EV_ABS, abs);
		return 1;
	}

	return 0;
}
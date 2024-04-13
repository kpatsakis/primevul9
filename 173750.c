static int sixaxis_mapping(struct hid_device *hdev, struct hid_input *hi,
			  struct hid_field *field, struct hid_usage *usage,
			  unsigned long **bit, int *max)
{
	if ((usage->hid & HID_USAGE_PAGE) == HID_UP_BUTTON) {
		unsigned int key = usage->hid & HID_USAGE;

		if (key >= ARRAY_SIZE(sixaxis_keymap))
			return -1;

		key = sixaxis_keymap[key];
		hid_map_usage_clear(hi, usage, bit, max, EV_KEY, key);
		return 1;
	} else if (usage->hid == HID_GD_POINTER) {
		/* The DS3 provides analog values for most buttons and even
		 * for HAT axes through GD Pointer. L2 and R2 are reported
		 * among these as well instead of as GD Z / RZ. Remap L2
		 * and R2 and ignore other analog 'button axes' as there is
		 * no good way for reporting them.
		 */
		switch (usage->usage_index) {
		case 8: /* L2 */
			usage->hid = HID_GD_Z;
			break;
		case 9: /* R2 */
			usage->hid = HID_GD_RZ;
			break;
		default:
			return -1;
		}

		hid_map_usage_clear(hi, usage, bit, max, EV_ABS, usage->hid & 0xf);
		return 1;
	} else if ((usage->hid & HID_USAGE_PAGE) == HID_UP_GENDESK) {
		unsigned int abs = usage->hid & HID_USAGE;

		if (abs >= ARRAY_SIZE(sixaxis_absmap))
			return -1;

		abs = sixaxis_absmap[abs];

		hid_map_usage_clear(hi, usage, bit, max, EV_ABS, abs);
		return 1;
	}

	return -1;
}
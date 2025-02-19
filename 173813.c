static int ms_ergonomy_kb_quirk(struct hid_input *hi, struct hid_usage *usage,
		unsigned long **bit, int *max)
{
	struct input_dev *input = hi->input;

	if ((usage->hid & HID_USAGE_PAGE) == HID_UP_CONSUMER) {
		switch (usage->hid & HID_USAGE) {
		/*
		 * Microsoft uses these 2 reserved usage ids for 2 keys on
		 * the MS office kb labelled "Office Home" and "Task Pane".
		 */
		case 0x29d:
			ms_map_key_clear(KEY_PROG1);
			return 1;
		case 0x29e:
			ms_map_key_clear(KEY_PROG2);
			return 1;
		}
		return 0;
	}

	if ((usage->hid & HID_USAGE_PAGE) != HID_UP_MSVENDOR)
		return 0;

	switch (usage->hid & HID_USAGE) {
	case 0xfd06: ms_map_key_clear(KEY_CHAT);	break;
	case 0xfd07: ms_map_key_clear(KEY_PHONE);	break;
	case 0xff00:
		/* Special keypad keys */
		ms_map_key_clear(KEY_KPEQUAL);
		set_bit(KEY_KPLEFTPAREN, input->keybit);
		set_bit(KEY_KPRIGHTPAREN, input->keybit);
		break;
	case 0xff01:
		/* Scroll wheel */
		hid_map_usage_clear(hi, usage, bit, max, EV_REL, REL_WHEEL);
		break;
	case 0xff02:
		/*
		 * This byte contains a copy of the modifier keys byte of a
		 * standard hid keyboard report, as send by interface 0
		 * (this usage is found on interface 1).
		 *
		 * This byte only gets send when another key in the same report
		 * changes state, and as such is useless, ignore it.
		 */
		return -1;
	case 0xff05:
		set_bit(EV_REP, input->evbit);
		ms_map_key_clear(KEY_F13);
		set_bit(KEY_F14, input->keybit);
		set_bit(KEY_F15, input->keybit);
		set_bit(KEY_F16, input->keybit);
		set_bit(KEY_F17, input->keybit);
		set_bit(KEY_F18, input->keybit);
		break;
	default:
		return 0;
	}
	return 1;
}
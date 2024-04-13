static void buzz_set_leds(struct sony_sc *sc)
{
	struct hid_device *hdev = sc->hdev;
	struct list_head *report_list =
		&hdev->report_enum[HID_OUTPUT_REPORT].report_list;
	struct hid_report *report = list_entry(report_list->next,
		struct hid_report, list);
	s32 *value = report->field[0]->value;

	BUILD_BUG_ON(MAX_LEDS < 4);

	value[0] = 0x00;
	value[1] = sc->led_state[0] ? 0xff : 0x00;
	value[2] = sc->led_state[1] ? 0xff : 0x00;
	value[3] = sc->led_state[2] ? 0xff : 0x00;
	value[4] = sc->led_state[3] ? 0xff : 0x00;
	value[5] = 0x00;
	value[6] = 0x00;
	hid_hw_request(hdev, report, HID_REQ_SET_REPORT);
}
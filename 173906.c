static void hid_lg3ff_set_autocenter(struct input_dev *dev, u16 magnitude)
{
	struct hid_device *hid = input_get_drvdata(dev);
	struct list_head *report_list = &hid->report_enum[HID_OUTPUT_REPORT].report_list;
	struct hid_report *report = list_entry(report_list->next, struct hid_report, list);

/*
 * Auto Centering probed from device
 * NOTE: deadman's switch on G940 must be covered
 * for effects to work
 */
	report->field[0]->value[0] = 0x51;
	report->field[0]->value[1] = 0x00;
	report->field[0]->value[2] = 0x00;
	report->field[0]->value[3] = 0x7F;
	report->field[0]->value[4] = 0x7F;
	report->field[0]->value[31] = 0x00;
	report->field[0]->value[32] = 0x00;
	report->field[0]->value[33] = 0x7F;
	report->field[0]->value[34] = 0x7F;

	hid_hw_request(hid, report, HID_REQ_SET_REPORT);
}
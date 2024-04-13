static void hid_lgff_set_autocenter(struct input_dev *dev, u16 magnitude)
{
	struct hid_device *hid = input_get_drvdata(dev);
	struct list_head *report_list = &hid->report_enum[HID_OUTPUT_REPORT].report_list;
	struct hid_report *report = list_entry(report_list->next, struct hid_report, list);
	__s32 *value = report->field[0]->value;
	magnitude = (magnitude >> 12) & 0xf;
	*value++ = 0xfe;
	*value++ = 0x0d;
	*value++ = magnitude;   /* clockwise strength */
	*value++ = magnitude;   /* counter-clockwise strength */
	*value++ = 0x80;
	*value++ = 0x00;
	*value = 0x00;
	hid_hw_request(hid, report, HID_REQ_SET_REPORT);
}
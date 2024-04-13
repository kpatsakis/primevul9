static bool hidpp_application_equals(struct hid_device *hdev,
				     unsigned int application)
{
	struct list_head *report_list;
	struct hid_report *report;

	report_list = &hdev->report_enum[HID_INPUT_REPORT].report_list;
	report = list_first_entry_or_null(report_list, struct hid_report, list);
	return report && report->application == application;
}
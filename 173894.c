static bool hidpp_validate_report(struct hid_device *hdev, int id,
				  int expected_length, bool optional)
{
	int report_length;

	if (id >= HID_MAX_IDS || id < 0) {
		hid_err(hdev, "invalid HID report id %u\n", id);
		return false;
	}

	report_length = hidpp_get_report_length(hdev, id);
	if (!report_length)
		return optional;

	if (report_length < expected_length) {
		hid_warn(hdev, "not enough values in hidpp report %d\n", id);
		return false;
	}

	return true;
}
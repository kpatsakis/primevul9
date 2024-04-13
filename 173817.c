static bool hidpp_validate_device(struct hid_device *hdev)
{
	return hidpp_validate_report(hdev, REPORT_ID_HIDPP_SHORT,
				     HIDPP_REPORT_SHORT_LENGTH, false) &&
	       hidpp_validate_report(hdev, REPORT_ID_HIDPP_LONG,
				     HIDPP_REPORT_LONG_LENGTH, true);
}
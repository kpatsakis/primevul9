static int __hidpp_send_report(struct hid_device *hdev,
				struct hidpp_report *hidpp_report)
{
	struct hidpp_device *hidpp = hid_get_drvdata(hdev);
	int fields_count, ret;

	switch (hidpp_report->report_id) {
	case REPORT_ID_HIDPP_SHORT:
		fields_count = HIDPP_REPORT_SHORT_LENGTH;
		break;
	case REPORT_ID_HIDPP_LONG:
		fields_count = HIDPP_REPORT_LONG_LENGTH;
		break;
	case REPORT_ID_HIDPP_VERY_LONG:
		fields_count = hidpp->very_long_report_length;
		break;
	default:
		return -ENODEV;
	}

	/*
	 * set the device_index as the receiver, it will be overwritten by
	 * hid_hw_request if needed
	 */
	hidpp_report->device_index = 0xff;

	if (hidpp->quirks & HIDPP_QUIRK_FORCE_OUTPUT_REPORTS) {
		ret = hid_hw_output_report(hdev, (u8 *)hidpp_report, fields_count);
	} else {
		ret = hid_hw_raw_request(hdev, hidpp_report->report_id,
			(u8 *)hidpp_report, fields_count, HID_OUTPUT_REPORT,
			HID_REQ_SET_REPORT);
	}

	return ret == fields_count ? 0 : -1;
}
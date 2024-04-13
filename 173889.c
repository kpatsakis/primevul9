static u8 *hidpp_report_fixup(struct hid_device *hdev, u8 *rdesc,
			      unsigned int *rsize)
{
	struct hidpp_device *hidpp = hid_get_drvdata(hdev);

	if (!hidpp)
		return rdesc;

	/* For 27 MHz keyboards the quirk gets set after hid_parse. */
	if (hdev->group == HID_GROUP_LOGITECH_27MHZ_DEVICE ||
	    (hidpp->quirks & HIDPP_QUIRK_HIDPP_CONSUMER_VENDOR_KEYS))
		rdesc = hidpp10_consumer_keys_report_fixup(hidpp, rdesc, rsize);

	return rdesc;
}
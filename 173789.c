static int sixaxis_set_operational_bt(struct hid_device *hdev)
{
	static const u8 report[] = { 0xf4, 0x42, 0x03, 0x00, 0x00 };
	u8 *buf;
	int ret;

	buf = kmemdup(report, sizeof(report), GFP_KERNEL);
	if (!buf)
		return -ENOMEM;

	ret = hid_hw_raw_request(hdev, buf[0], buf, sizeof(report),
				  HID_FEATURE_REPORT, HID_REQ_SET_REPORT);

	kfree(buf);

	return ret;
}
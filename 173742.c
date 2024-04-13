static int sixaxis_set_operational_usb(struct hid_device *hdev)
{
	struct sony_sc *sc = hid_get_drvdata(hdev);
	const int buf_size =
		max(SIXAXIS_REPORT_0xF2_SIZE, SIXAXIS_REPORT_0xF5_SIZE);
	u8 *buf;
	int ret;

	buf = kmalloc(buf_size, GFP_KERNEL);
	if (!buf)
		return -ENOMEM;

	ret = hid_hw_raw_request(hdev, 0xf2, buf, SIXAXIS_REPORT_0xF2_SIZE,
				 HID_FEATURE_REPORT, HID_REQ_GET_REPORT);
	if (ret < 0) {
		hid_err(hdev, "can't set operational mode: step 1\n");
		goto out;
	}

	/*
	 * Some compatible controllers like the Speedlink Strike FX and
	 * Gasia need another query plus an USB interrupt to get operational.
	 */
	ret = hid_hw_raw_request(hdev, 0xf5, buf, SIXAXIS_REPORT_0xF5_SIZE,
				 HID_FEATURE_REPORT, HID_REQ_GET_REPORT);
	if (ret < 0) {
		hid_err(hdev, "can't set operational mode: step 2\n");
		goto out;
	}

	/*
	 * But the USB interrupt would cause SHANWAN controllers to
	 * start rumbling non-stop, so skip step 3 for these controllers.
	 */
	if (sc->quirks & SHANWAN_GAMEPAD)
		goto out;

	ret = hid_hw_output_report(hdev, buf, 1);
	if (ret < 0) {
		hid_info(hdev, "can't set operational mode: step 3, ignoring\n");
		ret = 0;
	}

out:
	kfree(buf);

	return ret;
}
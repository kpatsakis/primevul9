static int ax_probe(struct hid_device *hdev, const struct hid_device_id *id)
{
	int error;

	dev_dbg(&hdev->dev, "ACRUX HID hardware probe...\n");

	error = hid_parse(hdev);
	if (error) {
		hid_err(hdev, "parse failed\n");
		return error;
	}

	error = hid_hw_start(hdev, HID_CONNECT_DEFAULT & ~HID_CONNECT_FF);
	if (error) {
		hid_err(hdev, "hw start failed\n");
		return error;
	}

	error = axff_init(hdev);
	if (error) {
		/*
		 * Do not fail device initialization completely as device
		 * may still be partially operable, just warn.
		 */
		hid_warn(hdev,
			 "Failed to enable force feedback support, error: %d\n",
			 error);
	}

	/*
	 * We need to start polling device right away, otherwise
	 * it will go into a coma.
	 */
	error = hid_hw_open(hdev);
	if (error) {
		dev_err(&hdev->dev, "hw open failed\n");
		hid_hw_stop(hdev);
		return error;
	}

	return 0;
}
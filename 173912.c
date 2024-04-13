static int ga_probe(struct hid_device *hdev, const struct hid_device_id *id)
{
	int ret;

	dev_dbg(&hdev->dev, "Greenasia HID hardware probe...");

	ret = hid_parse(hdev);
	if (ret) {
		hid_err(hdev, "parse failed\n");
		goto err;
	}

	ret = hid_hw_start(hdev, HID_CONNECT_DEFAULT & ~HID_CONNECT_FF);
	if (ret) {
		hid_err(hdev, "hw start failed\n");
		goto err;
	}

	gaff_init(hdev);

	return 0;
err:
	return ret;
}
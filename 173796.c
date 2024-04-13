static int dr_probe(struct hid_device *hdev, const struct hid_device_id *id)
{
	int ret;

	dev_dbg(&hdev->dev, "DragonRise Inc. HID hardware probe...");

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

	switch (hdev->product) {
	case 0x0006:
		ret = drff_init(hdev);
		if (ret) {
			dev_err(&hdev->dev, "force feedback init failed\n");
			hid_hw_stop(hdev);
			goto err;
		}
		break;
	}

	return 0;
err:
	return ret;
}
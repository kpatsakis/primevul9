static int ms_probe(struct hid_device *hdev, const struct hid_device_id *id)
{
	unsigned long quirks = id->driver_data;
	struct ms_data *ms;
	int ret;

	ms = devm_kzalloc(&hdev->dev, sizeof(*ms), GFP_KERNEL);
	if (ms == NULL)
		return -ENOMEM;

	ms->quirks = quirks;

	hid_set_drvdata(hdev, ms);

	if (quirks & MS_NOGET)
		hdev->quirks |= HID_QUIRK_NOGET;

	if (quirks & MS_SURFACE_DIAL)
		hdev->quirks |= HID_QUIRK_INPUT_PER_APP;

	ret = hid_parse(hdev);
	if (ret) {
		hid_err(hdev, "parse failed\n");
		goto err_free;
	}

	ret = hid_hw_start(hdev, HID_CONNECT_DEFAULT | ((quirks & MS_HIDINPUT) ?
				HID_CONNECT_HIDINPUT_FORCE : 0));
	if (ret) {
		hid_err(hdev, "hw start failed\n");
		goto err_free;
	}

	ret = ms_init_ff(hdev);
	if (ret)
		hid_err(hdev, "could not initialize ff, continuing anyway");

	return 0;
err_free:
	return ret;
}
static int hidpp_probe(struct hid_device *hdev, const struct hid_device_id *id)
{
	struct hidpp_device *hidpp;
	int ret;
	bool connected;
	unsigned int connect_mask = HID_CONNECT_DEFAULT;

	/* report_fixup needs drvdata to be set before we call hid_parse */
	hidpp = devm_kzalloc(&hdev->dev, sizeof(*hidpp), GFP_KERNEL);
	if (!hidpp)
		return -ENOMEM;

	hidpp->hid_dev = hdev;
	hidpp->name = hdev->name;
	hidpp->quirks = id->driver_data;
	hid_set_drvdata(hdev, hidpp);

	ret = hid_parse(hdev);
	if (ret) {
		hid_err(hdev, "%s:parse failed\n", __func__);
		return ret;
	}

	/*
	 * Make sure the device is HID++ capable, otherwise treat as generic HID
	 */
	if (!hidpp_validate_device(hdev)) {
		hid_set_drvdata(hdev, NULL);
		devm_kfree(&hdev->dev, hidpp);
		return hid_hw_start(hdev, HID_CONNECT_DEFAULT);
	}

	hidpp->very_long_report_length =
		hidpp_get_report_length(hdev, REPORT_ID_HIDPP_VERY_LONG);
	if (hidpp->very_long_report_length > HIDPP_REPORT_VERY_LONG_MAX_LENGTH)
		hidpp->very_long_report_length = HIDPP_REPORT_VERY_LONG_MAX_LENGTH;

	if (id->group == HID_GROUP_LOGITECH_DJ_DEVICE)
		hidpp->quirks |= HIDPP_QUIRK_UNIFYING;

	if (id->group == HID_GROUP_LOGITECH_27MHZ_DEVICE &&
	    hidpp_application_equals(hdev, HID_GD_MOUSE))
		hidpp->quirks |= HIDPP_QUIRK_HIDPP_WHEELS |
				 HIDPP_QUIRK_HIDPP_EXTRA_MOUSE_BTNS;

	if (id->group == HID_GROUP_LOGITECH_27MHZ_DEVICE &&
	    hidpp_application_equals(hdev, HID_GD_KEYBOARD))
		hidpp->quirks |= HIDPP_QUIRK_HIDPP_CONSUMER_VENDOR_KEYS;

	if (disable_raw_mode) {
		hidpp->quirks &= ~HIDPP_QUIRK_CLASS_WTP;
		hidpp->quirks &= ~HIDPP_QUIRK_NO_HIDINPUT;
	}

	if (hidpp->quirks & HIDPP_QUIRK_CLASS_WTP) {
		ret = wtp_allocate(hdev, id);
		if (ret)
			return ret;
	} else if (hidpp->quirks & HIDPP_QUIRK_CLASS_K400) {
		ret = k400_allocate(hdev);
		if (ret)
			return ret;
	}

	INIT_WORK(&hidpp->work, delayed_work_cb);
	mutex_init(&hidpp->send_mutex);
	init_waitqueue_head(&hidpp->wait);

	/* indicates we are handling the battery properties in the kernel */
	ret = sysfs_create_group(&hdev->dev.kobj, &ps_attribute_group);
	if (ret)
		hid_warn(hdev, "Cannot allocate sysfs group for %s\n",
			 hdev->name);

	/*
	 * Plain USB connections need to actually call start and open
	 * on the transport driver to allow incoming data.
	 */
	ret = hid_hw_start(hdev, 0);
	if (ret) {
		hid_err(hdev, "hw start failed\n");
		goto hid_hw_start_fail;
	}

	ret = hid_hw_open(hdev);
	if (ret < 0) {
		dev_err(&hdev->dev, "%s:hid_hw_open returned error:%d\n",
			__func__, ret);
		hid_hw_stop(hdev);
		goto hid_hw_open_fail;
	}

	/* Allow incoming packets */
	hid_device_io_start(hdev);

	if (hidpp->quirks & HIDPP_QUIRK_UNIFYING)
		hidpp_unifying_init(hidpp);

	connected = hidpp_root_get_protocol_version(hidpp) == 0;
	atomic_set(&hidpp->connected, connected);
	if (!(hidpp->quirks & HIDPP_QUIRK_UNIFYING)) {
		if (!connected) {
			ret = -ENODEV;
			hid_err(hdev, "Device not connected");
			goto hid_hw_init_fail;
		}

		hidpp_overwrite_name(hdev);
	}

	if (connected && (hidpp->quirks & HIDPP_QUIRK_CLASS_WTP)) {
		ret = wtp_get_config(hidpp);
		if (ret)
			goto hid_hw_init_fail;
	} else if (connected && (hidpp->quirks & HIDPP_QUIRK_CLASS_G920)) {
		ret = g920_get_config(hidpp);
		if (ret)
			goto hid_hw_init_fail;
	}

	hidpp_connect_event(hidpp);

	/* Reset the HID node state */
	hid_device_io_stop(hdev);
	hid_hw_close(hdev);
	hid_hw_stop(hdev);

	if (hidpp->quirks & HIDPP_QUIRK_NO_HIDINPUT)
		connect_mask &= ~HID_CONNECT_HIDINPUT;

	/* Now export the actual inputs and hidraw nodes to the world */
	ret = hid_hw_start(hdev, connect_mask);
	if (ret) {
		hid_err(hdev, "%s:hid_hw_start returned error\n", __func__);
		goto hid_hw_start_fail;
	}

	return ret;

hid_hw_init_fail:
	hid_hw_close(hdev);
hid_hw_open_fail:
	hid_hw_stop(hdev);
hid_hw_start_fail:
	sysfs_remove_group(&hdev->dev.kobj, &ps_attribute_group);
	cancel_work_sync(&hidpp->work);
	mutex_destroy(&hidpp->send_mutex);
	return ret;
}
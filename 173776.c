static void hidpp_overwrite_name(struct hid_device *hdev)
{
	struct hidpp_device *hidpp = hid_get_drvdata(hdev);
	char *name;

	if (hidpp->protocol_major < 2)
		return;

	name = hidpp_get_device_name(hidpp);

	if (!name) {
		hid_err(hdev, "unable to retrieve the name of the device");
	} else {
		dbg_hid("HID++: Got name: %s\n", name);
		snprintf(hdev->name, sizeof(hdev->name), "%s", name);
	}

	kfree(name);
}
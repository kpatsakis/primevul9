static int hidpp_unifying_init(struct hidpp_device *hidpp)
{
	struct hid_device *hdev = hidpp->hid_dev;
	const char *name;
	u32 serial;
	int ret;

	ret = hidpp_unifying_get_serial(hidpp, &serial);
	if (ret)
		return ret;

	snprintf(hdev->uniq, sizeof(hdev->uniq), "%04x-%4phD",
		 hdev->product, &serial);
	dbg_hid("HID++ Unifying: Got serial: %s\n", hdev->uniq);

	name = hidpp_unifying_get_name(hidpp);
	if (!name)
		return -EIO;

	snprintf(hdev->name, sizeof(hdev->name), "%s", name);
	dbg_hid("HID++ Unifying: Got name: %s\n", name);

	kfree(name);
	return 0;
}
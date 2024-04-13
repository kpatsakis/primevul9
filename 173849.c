static int k400_connect(struct hid_device *hdev, bool connected)
{
	struct hidpp_device *hidpp = hid_get_drvdata(hdev);

	if (!disable_tap_to_click)
		return 0;

	return k400_disable_tap_to_click(hidpp);
}
static int wtp_allocate(struct hid_device *hdev, const struct hid_device_id *id)
{
	struct hidpp_device *hidpp = hid_get_drvdata(hdev);
	struct wtp_data *wd;

	wd = devm_kzalloc(&hdev->dev, sizeof(struct wtp_data),
			GFP_KERNEL);
	if (!wd)
		return -ENOMEM;

	hidpp->private_data = wd;

	return 0;
};
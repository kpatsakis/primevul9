static void hidpp_remove(struct hid_device *hdev)
{
	struct hidpp_device *hidpp = hid_get_drvdata(hdev);

	if (!hidpp)
		return hid_hw_stop(hdev);

	sysfs_remove_group(&hdev->dev.kobj, &ps_attribute_group);

	if (hidpp->quirks & HIDPP_QUIRK_CLASS_G920)
		hidpp_ff_deinit(hdev);

	hid_hw_stop(hdev);
	cancel_work_sync(&hidpp->work);
	mutex_destroy(&hidpp->send_mutex);
}
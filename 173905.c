static void sony_remove(struct hid_device *hdev)
{
	struct sony_sc *sc = hid_get_drvdata(hdev);

	hid_hw_close(hdev);

	if (sc->quirks & DUALSHOCK4_CONTROLLER_BT)
		device_remove_file(&sc->hdev->dev, &dev_attr_bt_poll_interval);

	if (sc->fw_version)
		device_remove_file(&sc->hdev->dev, &dev_attr_firmware_version);

	if (sc->hw_version)
		device_remove_file(&sc->hdev->dev, &dev_attr_hardware_version);

	sony_cancel_work_sync(sc);

	sony_remove_dev_list(sc);

	sony_release_device_id(sc);

	hid_hw_stop(hdev);
}
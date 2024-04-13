static enum led_brightness lg4ff_led_get_brightness(struct led_classdev *led_cdev)
{
	struct device *dev = led_cdev->dev->parent;
	struct hid_device *hid = to_hid_device(dev);
	struct lg_drv_data *drv_data = hid_get_drvdata(hid);
	struct lg4ff_device_entry *entry;
	int i, value = 0;

	if (!drv_data) {
		hid_err(hid, "Device data not found.");
		return LED_OFF;
	}

	entry = drv_data->device_props;

	if (!entry) {
		hid_err(hid, "Device properties not found.");
		return LED_OFF;
	}

	for (i = 0; i < 5; i++)
		if (led_cdev == entry->wdata.led[i]) {
			value = (entry->wdata.led_state >> i) & 1;
			break;
		}

	return value ? LED_FULL : LED_OFF;
}
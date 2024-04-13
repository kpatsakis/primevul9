static int sony_led_blink_set(struct led_classdev *led, unsigned long *delay_on,
				unsigned long *delay_off)
{
	struct device *dev = led->dev->parent;
	struct hid_device *hdev = to_hid_device(dev);
	struct sony_sc *drv_data = hid_get_drvdata(hdev);
	int n;
	u8 new_on, new_off;

	if (!drv_data) {
		hid_err(hdev, "No device data\n");
		return -EINVAL;
	}

	/* Max delay is 255 deciseconds or 2550 milliseconds */
	if (*delay_on > 2550)
		*delay_on = 2550;
	if (*delay_off > 2550)
		*delay_off = 2550;

	/* Blink at 1 Hz if both values are zero */
	if (!*delay_on && !*delay_off)
		*delay_on = *delay_off = 500;

	new_on = *delay_on / 10;
	new_off = *delay_off / 10;

	for (n = 0; n < drv_data->led_count; n++) {
		if (led == drv_data->leds[n])
			break;
	}

	/* This LED is not registered on this device */
	if (n >= drv_data->led_count)
		return -EINVAL;

	/* Don't schedule work if the values didn't change */
	if (new_on != drv_data->led_delay_on[n] ||
		new_off != drv_data->led_delay_off[n]) {
		drv_data->led_delay_on[n] = new_on;
		drv_data->led_delay_off[n] = new_off;
		sony_schedule_work(drv_data, SONY_WORKER_STATE);
	}

	return 0;
}
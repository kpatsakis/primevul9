static int sony_input_configured(struct hid_device *hdev,
					struct hid_input *hidinput)
{
	struct sony_sc *sc = hid_get_drvdata(hdev);
	int append_dev_id;
	int ret;

	ret = sony_set_device_id(sc);
	if (ret < 0) {
		hid_err(hdev, "failed to allocate the device id\n");
		goto err_stop;
	}

	ret = append_dev_id = sony_check_add(sc);
	if (ret < 0)
		goto err_stop;

	ret = sony_allocate_output_report(sc);
	if (ret < 0) {
		hid_err(hdev, "failed to allocate the output report buffer\n");
		goto err_stop;
	}

	if (sc->quirks & NAVIGATION_CONTROLLER_USB) {
		/*
		 * The Sony Sixaxis does not handle HID Output Reports on the
		 * Interrupt EP like it could, so we need to force HID Output
		 * Reports to use HID_REQ_SET_REPORT on the Control EP.
		 *
		 * There is also another issue about HID Output Reports via USB,
		 * the Sixaxis does not want the report_id as part of the data
		 * packet, so we have to discard buf[0] when sending the actual
		 * control message, even for numbered reports, humpf!
		 *
		 * Additionally, the Sixaxis on USB isn't properly initialized
		 * until the PS logo button is pressed and as such won't retain
		 * any state set by an output report, so the initial
		 * configuration report is deferred until the first input
		 * report arrives.
		 */
		hdev->quirks |= HID_QUIRK_NO_OUTPUT_REPORTS_ON_INTR_EP;
		hdev->quirks |= HID_QUIRK_SKIP_OUTPUT_REPORT_ID;
		sc->defer_initialization = 1;

		ret = sixaxis_set_operational_usb(hdev);
		if (ret < 0) {
			hid_err(hdev, "Failed to set controller into operational mode\n");
			goto err_stop;
		}

		sony_init_output_report(sc, sixaxis_send_output_report);
	} else if (sc->quirks & NAVIGATION_CONTROLLER_BT) {
		/*
		 * The Navigation controller wants output reports sent on the ctrl
		 * endpoint when connected via Bluetooth.
		 */
		hdev->quirks |= HID_QUIRK_NO_OUTPUT_REPORTS_ON_INTR_EP;

		ret = sixaxis_set_operational_bt(hdev);
		if (ret < 0) {
			hid_err(hdev, "Failed to set controller into operational mode\n");
			goto err_stop;
		}

		sony_init_output_report(sc, sixaxis_send_output_report);
	} else if (sc->quirks & SIXAXIS_CONTROLLER_USB) {
		/*
		 * The Sony Sixaxis does not handle HID Output Reports on the
		 * Interrupt EP and the device only becomes active when the
		 * PS button is pressed. See comment for Navigation controller
		 * above for more details.
		 */
		hdev->quirks |= HID_QUIRK_NO_OUTPUT_REPORTS_ON_INTR_EP;
		hdev->quirks |= HID_QUIRK_SKIP_OUTPUT_REPORT_ID;
		sc->defer_initialization = 1;

		ret = sixaxis_set_operational_usb(hdev);
		if (ret < 0) {
			hid_err(hdev, "Failed to set controller into operational mode\n");
			goto err_stop;
		}

		ret = sony_register_sensors(sc);
		if (ret) {
			hid_err(sc->hdev,
			"Unable to initialize motion sensors: %d\n", ret);
			goto err_stop;
		}

		sony_init_output_report(sc, sixaxis_send_output_report);
	} else if (sc->quirks & SIXAXIS_CONTROLLER_BT) {
		/*
		 * The Sixaxis wants output reports sent on the ctrl endpoint
		 * when connected via Bluetooth.
		 */
		hdev->quirks |= HID_QUIRK_NO_OUTPUT_REPORTS_ON_INTR_EP;

		ret = sixaxis_set_operational_bt(hdev);
		if (ret < 0) {
			hid_err(hdev, "Failed to set controller into operational mode\n");
			goto err_stop;
		}

		ret = sony_register_sensors(sc);
		if (ret) {
			hid_err(sc->hdev,
			"Unable to initialize motion sensors: %d\n", ret);
			goto err_stop;
		}

		sony_init_output_report(sc, sixaxis_send_output_report);
	} else if (sc->quirks & DUALSHOCK4_CONTROLLER) {
		ret = dualshock4_get_calibration_data(sc);
		if (ret < 0) {
			hid_err(hdev, "Failed to get calibration data from Dualshock 4\n");
			goto err_stop;
		}

		ret = dualshock4_get_version_info(sc);
		if (ret < 0) {
			hid_err(sc->hdev, "Failed to get version data from Dualshock 4\n");
			goto err_stop;
		}

		ret = device_create_file(&sc->hdev->dev, &dev_attr_firmware_version);
		if (ret) {
			/* Make zero for cleanup reasons of sysfs entries. */
			sc->fw_version = 0;
			sc->hw_version = 0;
			hid_err(sc->hdev, "can't create sysfs firmware_version attribute err: %d\n", ret);
			goto err_stop;
		}

		ret = device_create_file(&sc->hdev->dev, &dev_attr_hardware_version);
		if (ret) {
			sc->hw_version = 0;
			hid_err(sc->hdev, "can't create sysfs hardware_version attribute err: %d\n", ret);
			goto err_stop;
		}

		/*
		 * The Dualshock 4 touchpad supports 2 touches and has a
		 * resolution of 1920x942 (44.86 dots/mm).
		 */
		ret = sony_register_touchpad(sc, 2, 1920, 942, 0, 0, 0);
		if (ret) {
			hid_err(sc->hdev,
			"Unable to initialize multi-touch slots: %d\n",
			ret);
			goto err_stop;
		}

		ret = sony_register_sensors(sc);
		if (ret) {
			hid_err(sc->hdev,
			"Unable to initialize motion sensors: %d\n", ret);
			goto err_stop;
		}

		if (sc->quirks & DUALSHOCK4_CONTROLLER_BT) {
			sc->ds4_bt_poll_interval = DS4_BT_DEFAULT_POLL_INTERVAL_MS;
			ret = device_create_file(&sc->hdev->dev, &dev_attr_bt_poll_interval);
			if (ret)
				hid_warn(sc->hdev,
				 "can't create sysfs bt_poll_interval attribute err: %d\n",
				 ret);
		}

		if (sc->quirks & DUALSHOCK4_DONGLE) {
			INIT_WORK(&sc->hotplug_worker, dualshock4_calibration_work);
			sc->hotplug_worker_initialized = 1;
			sc->ds4_dongle_state = DONGLE_DISCONNECTED;
		}

		sony_init_output_report(sc, dualshock4_send_output_report);
	} else if (sc->quirks & NSG_MRXU_REMOTE) {
		/*
		 * The NSG-MRxU touchpad supports 2 touches and has a
		 * resolution of 1667x1868
		 */
		ret = sony_register_touchpad(sc, 2,
			NSG_MRXU_MAX_X, NSG_MRXU_MAX_Y, 15, 15, 1);
		if (ret) {
			hid_err(sc->hdev,
			"Unable to initialize multi-touch slots: %d\n",
			ret);
			goto err_stop;
		}

	} else if (sc->quirks & MOTION_CONTROLLER) {
		sony_init_output_report(sc, motion_send_output_report);
	} else {
		ret = 0;
	}

	if (sc->quirks & SONY_LED_SUPPORT) {
		ret = sony_leds_init(sc);
		if (ret < 0)
			goto err_stop;
	}

	if (sc->quirks & SONY_BATTERY_SUPPORT) {
		ret = sony_battery_probe(sc, append_dev_id);
		if (ret < 0)
			goto err_stop;

		/* Open the device to receive reports with battery info */
		ret = hid_hw_open(hdev);
		if (ret < 0) {
			hid_err(hdev, "hw open failed\n");
			goto err_stop;
		}
	}

	if (sc->quirks & SONY_FF_SUPPORT) {
		ret = sony_init_ff(sc);
		if (ret < 0)
			goto err_close;
	}

	return 0;
err_close:
	hid_hw_close(hdev);
err_stop:
	/* Piggy back on the default ds4_bt_ poll_interval to determine
	 * if we need to remove the file as we don't know for sure if we
	 * executed that logic.
	 */
	if (sc->ds4_bt_poll_interval)
		device_remove_file(&sc->hdev->dev, &dev_attr_bt_poll_interval);
	if (sc->fw_version)
		device_remove_file(&sc->hdev->dev, &dev_attr_firmware_version);
	if (sc->hw_version)
		device_remove_file(&sc->hdev->dev, &dev_attr_hardware_version);
	sony_cancel_work_sync(sc);
	sony_remove_dev_list(sc);
	sony_release_device_id(sc);
	return ret;
}
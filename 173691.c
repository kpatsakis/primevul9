static int sony_battery_probe(struct sony_sc *sc, int append_dev_id)
{
	const char *battery_str_fmt = append_dev_id ?
		"sony_controller_battery_%pMR_%i" :
		"sony_controller_battery_%pMR";
	struct power_supply_config psy_cfg = { .drv_data = sc, };
	struct hid_device *hdev = sc->hdev;
	int ret;

	/*
	 * Set the default battery level to 100% to avoid low battery warnings
	 * if the battery is polled before the first device report is received.
	 */
	sc->battery_capacity = 100;

	sc->battery_desc.properties = sony_battery_props;
	sc->battery_desc.num_properties = ARRAY_SIZE(sony_battery_props);
	sc->battery_desc.get_property = sony_battery_get_property;
	sc->battery_desc.type = POWER_SUPPLY_TYPE_BATTERY;
	sc->battery_desc.use_for_apm = 0;
	sc->battery_desc.name = devm_kasprintf(&hdev->dev, GFP_KERNEL,
					  battery_str_fmt, sc->mac_address, sc->device_id);
	if (!sc->battery_desc.name)
		return -ENOMEM;

	sc->battery = devm_power_supply_register(&hdev->dev, &sc->battery_desc,
					    &psy_cfg);
	if (IS_ERR(sc->battery)) {
		ret = PTR_ERR(sc->battery);
		hid_err(hdev, "Unable to register battery device\n");
		return ret;
	}

	power_supply_powers(sc->battery, &hdev->dev);
	return 0;
}
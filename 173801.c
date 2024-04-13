static int hidpp_initialize_battery(struct hidpp_device *hidpp)
{
	static atomic_t battery_no = ATOMIC_INIT(0);
	struct power_supply_config cfg = { .drv_data = hidpp };
	struct power_supply_desc *desc = &hidpp->battery.desc;
	enum power_supply_property *battery_props;
	struct hidpp_battery *battery;
	unsigned int num_battery_props;
	unsigned long n;
	int ret;

	if (hidpp->battery.ps)
		return 0;

	hidpp->battery.feature_index = 0xff;
	hidpp->battery.solar_feature_index = 0xff;

	if (hidpp->protocol_major >= 2) {
		if (hidpp->quirks & HIDPP_QUIRK_CLASS_K750)
			ret = hidpp_solar_request_battery_event(hidpp);
		else
			ret = hidpp20_query_battery_info(hidpp);

		if (ret)
			return ret;
		hidpp->capabilities |= HIDPP_CAPABILITY_HIDPP20_BATTERY;
	} else {
		ret = hidpp10_query_battery_status(hidpp);
		if (ret) {
			ret = hidpp10_query_battery_mileage(hidpp);
			if (ret)
				return -ENOENT;
			hidpp->capabilities |= HIDPP_CAPABILITY_BATTERY_MILEAGE;
		} else {
			hidpp->capabilities |= HIDPP_CAPABILITY_BATTERY_LEVEL_STATUS;
		}
		hidpp->capabilities |= HIDPP_CAPABILITY_HIDPP10_BATTERY;
	}

	battery_props = devm_kmemdup(&hidpp->hid_dev->dev,
				     hidpp_battery_props,
				     sizeof(hidpp_battery_props),
				     GFP_KERNEL);
	if (!battery_props)
		return -ENOMEM;

	num_battery_props = ARRAY_SIZE(hidpp_battery_props) - 2;

	if (hidpp->capabilities & HIDPP_CAPABILITY_BATTERY_MILEAGE)
		battery_props[num_battery_props++] =
				POWER_SUPPLY_PROP_CAPACITY;

	if (hidpp->capabilities & HIDPP_CAPABILITY_BATTERY_LEVEL_STATUS)
		battery_props[num_battery_props++] =
				POWER_SUPPLY_PROP_CAPACITY_LEVEL;

	battery = &hidpp->battery;

	n = atomic_inc_return(&battery_no) - 1;
	desc->properties = battery_props;
	desc->num_properties = num_battery_props;
	desc->get_property = hidpp_battery_get_property;
	sprintf(battery->name, "hidpp_battery_%ld", n);
	desc->name = battery->name;
	desc->type = POWER_SUPPLY_TYPE_BATTERY;
	desc->use_for_apm = 0;

	battery->ps = devm_power_supply_register(&hidpp->hid_dev->dev,
						 &battery->desc,
						 &cfg);
	if (IS_ERR(battery->ps))
		return PTR_ERR(battery->ps);

	power_supply_powers(battery->ps, &hidpp->hid_dev->dev);

	return ret;
}
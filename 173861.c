static int hidpp20_query_battery_info(struct hidpp_device *hidpp)
{
	u8 feature_type;
	int ret;
	int status, capacity, next_capacity, level;

	if (hidpp->battery.feature_index == 0xff) {
		ret = hidpp_root_get_feature(hidpp,
					     HIDPP_PAGE_BATTERY_LEVEL_STATUS,
					     &hidpp->battery.feature_index,
					     &feature_type);
		if (ret)
			return ret;
	}

	ret = hidpp20_batterylevel_get_battery_capacity(hidpp,
						hidpp->battery.feature_index,
						&status, &capacity,
						&next_capacity, &level);
	if (ret)
		return ret;

	ret = hidpp20_batterylevel_get_battery_info(hidpp,
						hidpp->battery.feature_index);
	if (ret)
		return ret;

	hidpp->battery.status = status;
	hidpp->battery.capacity = capacity;
	hidpp->battery.level = level;
	/* the capacity is only available when discharging or full */
	hidpp->battery.online = status == POWER_SUPPLY_STATUS_DISCHARGING ||
				status == POWER_SUPPLY_STATUS_FULL;

	return 0;
}
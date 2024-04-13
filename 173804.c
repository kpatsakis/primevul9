static int hidpp20_battery_event(struct hidpp_device *hidpp,
				 u8 *data, int size)
{
	struct hidpp_report *report = (struct hidpp_report *)data;
	int status, capacity, next_capacity, level;
	bool changed;

	if (report->fap.feature_index != hidpp->battery.feature_index ||
	    report->fap.funcindex_clientid != EVENT_BATTERY_LEVEL_STATUS_BROADCAST)
		return 0;

	status = hidpp20_batterylevel_map_status_capacity(report->fap.params,
							  &capacity,
							  &next_capacity,
							  &level);

	/* the capacity is only available when discharging or full */
	hidpp->battery.online = status == POWER_SUPPLY_STATUS_DISCHARGING ||
				status == POWER_SUPPLY_STATUS_FULL;

	changed = capacity != hidpp->battery.capacity ||
		  level != hidpp->battery.level ||
		  status != hidpp->battery.status;

	if (changed) {
		hidpp->battery.level = level;
		hidpp->battery.capacity = capacity;
		hidpp->battery.status = status;
		if (hidpp->battery.ps)
			power_supply_changed(hidpp->battery.ps);
	}

	return 0;
}
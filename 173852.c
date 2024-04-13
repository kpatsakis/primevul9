static int hidpp10_battery_event(struct hidpp_device *hidpp, u8 *data, int size)
{
	struct hidpp_report *report = (struct hidpp_report *)data;
	int status, capacity, level;
	bool changed;

	if (report->report_id != REPORT_ID_HIDPP_SHORT)
		return 0;

	switch (report->rap.sub_id) {
	case HIDPP_REG_BATTERY_STATUS:
		capacity = hidpp->battery.capacity;
		level = hidpp10_battery_status_map_level(report->rawbytes[1]);
		status = hidpp10_battery_status_map_status(report->rawbytes[2]);
		break;
	case HIDPP_REG_BATTERY_MILEAGE:
		capacity = report->rap.params[0];
		level = hidpp->battery.level;
		status = hidpp10_battery_mileage_map_status(report->rawbytes[3]);
		break;
	default:
		return 0;
	}

	changed = capacity != hidpp->battery.capacity ||
		  level != hidpp->battery.level ||
		  status != hidpp->battery.status;

	/* the capacity is only available when discharging or full */
	hidpp->battery.online = status == POWER_SUPPLY_STATUS_DISCHARGING ||
				status == POWER_SUPPLY_STATUS_FULL;

	if (changed) {
		hidpp->battery.level = level;
		hidpp->battery.status = status;
		if (hidpp->battery.ps)
			power_supply_changed(hidpp->battery.ps);
	}

	return 0;
}
static int hidpp10_query_battery_status(struct hidpp_device *hidpp)
{
	struct hidpp_report response;
	int ret, status;

	ret = hidpp_send_rap_command_sync(hidpp,
					REPORT_ID_HIDPP_SHORT,
					HIDPP_GET_REGISTER,
					HIDPP_REG_BATTERY_STATUS,
					NULL, 0, &response);
	if (ret)
		return ret;

	hidpp->battery.level =
		hidpp10_battery_status_map_level(response.rap.params[0]);
	status = hidpp10_battery_status_map_status(response.rap.params[1]);
	hidpp->battery.status = status;
	/* the capacity is only available when discharging or full */
	hidpp->battery.online = status == POWER_SUPPLY_STATUS_DISCHARGING ||
				status == POWER_SUPPLY_STATUS_FULL;

	return 0;
}
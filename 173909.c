static int hidpp20_batterylevel_get_battery_capacity(struct hidpp_device *hidpp,
						     u8 feature_index,
						     int *status,
						     int *capacity,
						     int *next_capacity,
						     int *level)
{
	struct hidpp_report response;
	int ret;
	u8 *params = (u8 *)response.fap.params;

	ret = hidpp_send_fap_command_sync(hidpp, feature_index,
					  CMD_BATTERY_LEVEL_STATUS_GET_BATTERY_LEVEL_STATUS,
					  NULL, 0, &response);
	if (ret > 0) {
		hid_err(hidpp->hid_dev, "%s: received protocol error 0x%02x\n",
			__func__, ret);
		return -EPROTO;
	}
	if (ret)
		return ret;

	*status = hidpp20_batterylevel_map_status_capacity(params, capacity,
							   next_capacity,
							   level);

	return 0;
}
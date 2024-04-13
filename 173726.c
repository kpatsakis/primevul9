static int hidpp20_batterylevel_get_battery_info(struct hidpp_device *hidpp,
						  u8 feature_index)
{
	struct hidpp_report response;
	int ret;
	u8 *params = (u8 *)response.fap.params;
	unsigned int level_count, flags;

	ret = hidpp_send_fap_command_sync(hidpp, feature_index,
					  CMD_BATTERY_LEVEL_STATUS_GET_BATTERY_CAPABILITY,
					  NULL, 0, &response);
	if (ret > 0) {
		hid_err(hidpp->hid_dev, "%s: received protocol error 0x%02x\n",
			__func__, ret);
		return -EPROTO;
	}
	if (ret)
		return ret;

	level_count = params[0];
	flags = params[1];

	if (level_count < 10 || !(flags & FLAG_BATTERY_LEVEL_MILEAGE))
		hidpp->capabilities |= HIDPP_CAPABILITY_BATTERY_LEVEL_STATUS;
	else
		hidpp->capabilities |= HIDPP_CAPABILITY_BATTERY_MILEAGE;

	return 0;
}
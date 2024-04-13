static int hidpp_solar_request_battery_event(struct hidpp_device *hidpp)
{
	struct hidpp_report response;
	u8 params[2] = { 1, 1 };
	u8 feature_type;
	int ret;

	if (hidpp->battery.feature_index == 0xff) {
		ret = hidpp_root_get_feature(hidpp,
					     HIDPP_PAGE_SOLAR_KEYBOARD,
					     &hidpp->battery.solar_feature_index,
					     &feature_type);
		if (ret)
			return ret;
	}

	ret = hidpp_send_fap_command_sync(hidpp,
					  hidpp->battery.solar_feature_index,
					  CMD_SOLAR_SET_LIGHT_MEASURE,
					  params, 2, &response);
	if (ret > 0) {
		hid_err(hidpp->hid_dev, "%s: received protocol error 0x%02x\n",
			__func__, ret);
		return -EPROTO;
	}
	if (ret)
		return ret;

	hidpp->capabilities |= HIDPP_CAPABILITY_BATTERY_MILEAGE;

	return 0;
}
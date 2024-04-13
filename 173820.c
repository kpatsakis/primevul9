static int hidpp_hrw_set_wheel_mode(struct hidpp_device *hidpp, bool invert,
	bool high_resolution, bool use_hidpp)
{
	u8 feature_index;
	u8 feature_type;
	int ret;
	u8 params[1];
	struct hidpp_report response;

	ret = hidpp_root_get_feature(hidpp, HIDPP_PAGE_HIRES_WHEEL,
				     &feature_index, &feature_type);
	if (ret)
		return ret;

	params[0] = (invert          ? BIT(2) : 0) |
		    (high_resolution ? BIT(1) : 0) |
		    (use_hidpp       ? BIT(0) : 0);

	return hidpp_send_fap_command_sync(hidpp, feature_index,
					   CMD_HIRES_WHEEL_SET_WHEEL_MODE,
					   params, sizeof(params), &response);
}
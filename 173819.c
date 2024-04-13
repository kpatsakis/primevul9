static int hidpp_hrw_get_wheel_capability(struct hidpp_device *hidpp,
	u8 *multiplier)
{
	u8 feature_index;
	u8 feature_type;
	int ret;
	struct hidpp_report response;

	ret = hidpp_root_get_feature(hidpp, HIDPP_PAGE_HIRES_WHEEL,
				     &feature_index, &feature_type);
	if (ret)
		goto return_default;

	ret = hidpp_send_fap_command_sync(hidpp, feature_index,
					  CMD_HIRES_WHEEL_GET_WHEEL_CAPABILITY,
					  NULL, 0, &response);
	if (ret)
		goto return_default;

	*multiplier = response.fap.params[0];
	return 0;
return_default:
	hid_warn(hidpp->hid_dev,
		 "Couldn't get wheel multiplier (error %d)\n", ret);
	return ret;
}
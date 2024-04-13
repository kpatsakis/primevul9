static int hidpp_root_get_feature(struct hidpp_device *hidpp, u16 feature,
	u8 *feature_index, u8 *feature_type)
{
	struct hidpp_report response;
	int ret;
	u8 params[2] = { feature >> 8, feature & 0x00FF };

	ret = hidpp_send_fap_command_sync(hidpp,
			HIDPP_PAGE_ROOT_IDX,
			CMD_ROOT_GET_FEATURE,
			params, 2, &response);
	if (ret)
		return ret;

	if (response.fap.params[0] == 0)
		return -ENOENT;

	*feature_index = response.fap.params[0];
	*feature_type = response.fap.params[1];

	return ret;
}
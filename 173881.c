static int hidpp_touchpad_get_raw_info(struct hidpp_device *hidpp,
	u8 feature_index, struct hidpp_touchpad_raw_info *raw_info)
{
	struct hidpp_report response;
	int ret;
	u8 *params = (u8 *)response.fap.params;

	ret = hidpp_send_fap_command_sync(hidpp, feature_index,
		CMD_TOUCHPAD_GET_RAW_INFO, NULL, 0, &response);

	if (ret > 0) {
		hid_err(hidpp->hid_dev, "%s: received protocol error 0x%02x\n",
			__func__, ret);
		return -EPROTO;
	}
	if (ret)
		return ret;

	raw_info->x_size = get_unaligned_be16(&params[0]);
	raw_info->y_size = get_unaligned_be16(&params[2]);
	raw_info->z_range = params[4];
	raw_info->area_range = params[5];
	raw_info->maxcontacts = params[7];
	raw_info->origin = params[8];
	/* res is given in unit per inch */
	raw_info->res = get_unaligned_be16(&params[13]) * 2 / 51;

	return ret;
}